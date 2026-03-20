#include "tcp_mgr.h"
#include <spdlog/spdlog.h>
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include "user_mgr.h"

TCPMgr::TCPMgr()
    : flag_recv_pending_(false), host_(""), port_(0), data_len_(0), data_id_(0)
{
    // 将自身移到子线程
    this->moveToThread(&worker_thread_);

    // 线程启动后初始化 socket（确保 socket 在子线程中创建）
    QObject::connect(&worker_thread_, &QThread::started, this, &TCPMgr::initSocket);

    initHttpHandler();
    worker_thread_.start();
}

void TCPMgr::initSocket()
{
    socket_ = new QTcpSocket(this);

    QObject::connect(socket_, &QTcpSocket::connected, this, [this]()
                     {   SPDLOG_INFO("tcp connected");
                emit signTcpConnectionStatus(true); });

    QObject::connect(socket_, &QTcpSocket::readyRead, this, [this]()
                     {
        buffer_.append(socket_->readAll());
        QDataStream stream(&buffer_, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_6_8);

        forever
        {
            if (!flag_recv_pending_)
            {
                if (buffer_.size() < static_cast<int>(sizeof(quint16) * 2))
                {
                    return;
                }

                stream >> data_id_;
                stream >> data_len_;

                buffer_ = buffer_.mid(sizeof(quint16) * 2);
            }

            if (buffer_.size() < static_cast<int>(data_len_))
            {
                flag_recv_pending_ = true;
                return;
            }

            flag_recv_pending_ = false;
            QByteArray data_body = buffer_.mid(0, data_len_);
            buffer_ = buffer_.mid(data_len_);

            const auto id = static_cast<ReqId>(data_id_);
            const auto it = http_handlers_.find(id);
            if (it != http_handlers_.end())
            {
                it->second(id, data_len_, data_body);
            }
            else
            {
                SPDLOG_ERROR("No handler for id {}", static_cast<int>(id));
            }
        } });

    QObject::connect(socket_, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError err)
                     { SPDLOG_ERROR("tcp error: {}", static_cast<int>(err)); });

    QObject::connect(socket_, &QTcpSocket::disconnected, this, [this]()
                     { SPDLOG_INFO("tcp disconnected"); });
}

void TCPMgr::initHttpHandler()
{
    // 聊天登录回包后处理
    http_handlers_.insert({ReqId::ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data)
                           {
                               QJsonDocument json_doc = QJsonDocument::fromJson(data);

                               if (json_doc.isNull())
                               {
                                   SPDLOG_ERROR("Json parse failed!");
                                   return;
                               }

                               QJsonObject json_obj = json_doc.object();

                               if (!json_obj.contains("error"))
                               {
                                   emit signLoginChatStatus(false);
                                   return;
                               }

                               int erro_code = json_obj.value("error").toInt();
                               if (erro_code != 0)
                               {
                                   SPDLOG_ERROR("Chat login failed, error code: {}", erro_code);
                                   emit signLoginChatStatus(false);
                                   return;
                               }

                               auto uid = json_obj.value("uid").toInt();
                               auto name = json_obj.value("username").toString();
                               auto token = json_obj.value("token").toString();
                               auto icon = json_obj.value("icon").toString();
                               auto nick = json_obj.value("nick").toString();
                               auto sex = json_obj.value("sex").toInt();

                               auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);
                               UserMgr::getInstance().setUserInfo(user_info);
                               UserMgr::getInstance().setToken(token);

                               // 更新好友请求列表
                               if (json_obj.contains("apply_list"))
                               {
                                   auto apply_list = json_obj.value("apply_list").toArray();
                                   UserMgr::getInstance().appendApplyList(apply_list);
                               }

                               // 更新好友列表
                               if (json_obj.contains("friend_list"))
                               {
                                   auto friend_list = json_obj.value("friend_list").toArray();
                                   UserMgr::getInstance().appendContactList(friend_list);
                               }

                               SPDLOG_INFO("Chat login success, uid: {}, name: {}", uid, name.toStdString());

                               emit signLoginChatStatus(true);
                           }});

    // 搜索用户回包后处理
    http_handlers_.insert({ReqId::ID_SEARCH_USER_RSP, [this](ReqId id, int len, QByteArray data)
                           {
                               QJsonDocument json_doc = QJsonDocument::fromJson(data);

                               if (json_doc.isNull())
                               {
                                   SPDLOG_ERROR("Json parse failed!");
                                   return;
                               }

                               QJsonObject json_obj = json_doc.object();

                               if (!json_obj.contains("error"))
                               {
                                   return;
                               }

                               int erro_code = json_obj.value("error").toInt();
                               if (erro_code != 0)
                               {
                                   return;
                               }

                               auto search_info = std::make_shared<SearchInfo>(json_obj.value("uid").toInt(),
                                                                               json_obj.value("username").toString(),
                                                                               json_obj.value("nick").toString(),
                                                                               json_obj.value("desc").toString(),
                                                                               json_obj.value("sex").toInt(),
                                                                               json_obj.value("icon").toString());
                               emit signSearchUserResult(search_info);
                           }});

    // 发送好友申请后，服务器返回给自己的回包（A向B发申请 → 服务器回复A）
    http_handlers_.insert({ReqId::ID_ADD_FRIEND_RSP, [this](ReqId id, int len, QByteArray data)
                           {
                               QJsonDocument json_doc = QJsonDocument::fromJson(data);

                               if (json_doc.isNull())
                               {
                                   SPDLOG_ERROR("Json parse failed!");
                                   return;
                               }

                               QJsonObject json_obj = json_doc.object();

                               if (!json_obj.contains("error"))
                               {
                                   return;
                               }

                               int erro_code = json_obj.value("error").toInt();
                               if (erro_code != 0)
                               {
                                   return;
                               }

                               SPDLOG_INFO("Add friend success!");
                           }});

    // 收到他人的好友申请，服务器推送给被申请方的通知（A向B发申请 → 服务器通知B）
    http_handlers_.insert({ReqId::ID_NOTIFY_ADD_FRIEND_REQ, [this](ReqId id, int len, QByteArray data)
                           {
                               QJsonDocument json_doc = QJsonDocument::fromJson(data);

                               if (json_doc.isNull())
                               {
                                   SPDLOG_ERROR("Json parse failed!");
                                   return;
                               }

                               QJsonObject json_obj = json_doc.object();

                               if (!json_obj.contains("error"))
                               {
                                   return;
                               }

                               int erro_code = json_obj.value("error").toInt();
                               if (erro_code != 0)
                               {
                                   return;
                               }

                               auto from_uid = json_obj.value("apply_uid").toInt();
                               auto name = json_obj.value("username").toString();
                               auto desc = json_obj.value("desc").toString();
                               auto icon = json_obj.value("icon").toString();
                               auto nick = json_obj.value("nick").toString();
                               auto sex = json_obj.value("sex").toInt();

                               auto apply_info = std::make_shared<AddFriendApply>(from_uid, name, desc, icon, nick, sex);
                               emit signReceiveFriendApply(apply_info);

                               SPDLOG_INFO("Receive friend apply, from_uid: {}, name: {}", from_uid, name.toStdString());
                           }});

    // 同意好友申请后，服务器返回给自己的回包（A同意B的申请 → 服务器回复A）
    http_handlers_.insert({ReqId::ID_AUTH_FRIEND_RSP, [this](ReqId id, int len, QByteArray data)
                           {
                               QJsonDocument json_doc = QJsonDocument::fromJson(data);

                               if (json_doc.isNull())
                               {
                                   SPDLOG_ERROR("Json parse failed!");
                                   return;
                               }

                               QJsonObject json_obj = json_doc.object();

                               if (!json_obj.contains("error"))
                               {
                                   return;
                               }

                               int erro_code = json_obj.value("error").toInt();
                               if (erro_code != 0)
                               {
                                   return;
                               }

                               auto name = json_obj.value("username").toString();
                               auto nick = json_obj.value("nick").toString();
                               auto icon = json_obj.value("icon").toString();
                               auto sex = json_obj.value("sex").toInt();
                               auto uid = json_obj.value("uid").toInt();

                               auto friend_info = std::make_shared<FriendInfo>(uid, name, nick, icon, sex, "", "", "");
                               UserMgr::getInstance().addFriend(friend_info);
                               UserMgr::getInstance().removeApply(uid);
                               SPDLOG_INFO("friend auth success,contains name: {}, nick: {}, uid: {}", name.toStdString(), nick.toStdString(), uid);
                           }});

    // 对方同意好友申请后，服务器推送给申请发起方的通知（A同意B的申请 → 服务器通知B）
    http_handlers_.insert({ReqId::ID_NOTIFY_AUTH_FRIEND_REQ, [this](ReqId id, int len, QByteArray data)
                           {
                               QJsonDocument json_doc = QJsonDocument::fromJson(data);

                               if (json_doc.isNull())
                               {
                                   SPDLOG_ERROR("Json parse failed!");
                                   return;
                               }

                               QJsonObject json_obj = json_doc.object();

                               if (!json_obj.contains("error"))
                               {
                                   return;
                               }

                               int erro_code = json_obj.value("error").toInt();
                               if (erro_code != 0)
                               {
                                   return;
                               }

                               auto from_uid = json_obj.value("from_uid").toInt();
                               auto name = json_obj.value("username").toString();
                               auto desc = json_obj.value("desc").toString();
                               auto icon = json_obj.value("icon").toString();
                               auto nick = json_obj.value("nick").toString();
                               auto sex = json_obj.value("sex").toInt();

                               auto friend_info = std::make_shared<FriendInfo>(from_uid, name, nick, icon, sex, "", "", "");
                               UserMgr::getInstance().addFriend(friend_info);
                               SPDLOG_INFO("Peer approved friend request, from_uid: {}, name: {}", from_uid, name.toStdString());
                           }});

    // 发送消息后，服务器返回给自己的回包（A向B发消息 → 服务器回复A）
    http_handlers_.insert({ReqId::ID_TEXT_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data)
                           {
                               QJsonDocument json_doc = QJsonDocument::fromJson(data);

                               if (json_doc.isNull())
                               {
                                   SPDLOG_ERROR("Json parse failed!");
                                   return;
                               }

                               QJsonObject json_obj = json_doc.object();

                               if (!json_obj.contains("error"))
                               {
                                   return;
                               }

                               int erro_code = json_obj.value("error").toInt();
                               if (erro_code != 0)
                               {
                                   return;
                               }

                               auto from_uid = json_obj.value("from_uid").toInt();
                               auto to_uid = json_obj.value("to_uid").toInt();

                               SPDLOG_INFO("from_uid:{} Send message to to_uid:{} success!", from_uid, to_uid);
                           }});

    // 发送消息后，服务器推送给对方的通知（A向B发消息 → 服务器通知B）
    http_handlers_.insert({ReqId::ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, int len, QByteArray data)
                           {
                               QJsonDocument json_doc = QJsonDocument::fromJson(data);

                               if (json_doc.isNull())
                               {
                                   SPDLOG_ERROR("Json parse failed!");
                                   return;
                               }

                               QJsonObject json_obj = json_doc.object();

                               if (!json_obj.contains("error"))
                               {
                                   return;
                               }

                               int erro_code = json_obj.value("error").toInt();
                               if (erro_code != 0)
                               {
                                   return;
                               }

                               auto from_uid = json_obj.value("from_uid").toInt();
                               auto to_uid = json_obj.value("to_uid").toInt();

                               auto text_array = json_obj.value("text_array").toArray();
                               for (const auto &item : text_array)
                               {
                                   auto obj = item.toObject();
                                   auto msgid = obj.value("msgid").toString();
                                   auto content = obj.value("content").toString();

                                   auto &mgr = UserMgr::getInstance();
                                   auto *chat_model = mgr.chatListModel();
                                   // 如果会话不存在，自动创建
                                   if (!chat_model->hasChat(from_uid))
                                   {
                                       QString name = mgr.contactListModel()->getName(from_uid);
                                       QString avatar = mgr.contactListModel()->getAvatar(from_uid);
                                       auto info = std::make_shared<FriendInfo>(from_uid, name, name, avatar, 0, "", "", "");
                                       chat_model->addChat(info);
                                   }
                                   auto *msg_model = chat_model->getMessageModel(from_uid);
                                   if (msg_model)
                                   {
                                       msg_model->addMessage(msgid, content, QDateTime::currentDateTime().toString("hh:mm"), false);
                                       chat_model->incrementUnread(from_uid);
                                   }
                               }
                           }});
}

TCPMgr::~TCPMgr()
{
    worker_thread_.quit();
    worker_thread_.wait();
}

void TCPMgr::tcpConnection(const ServerInfo &info)
{
    if (socket_->state() != QAbstractSocket::UnconnectedState)
    {
        socket_->abort();
    }
    buffer_.clear();
    flag_recv_pending_ = false;
    host_ = info.host;
    port_ = static_cast<uint16_t>(std::stoi(info.port));
    socket_->connectToHost(host_.c_str(), port_);
}

void TCPMgr::tcpSendData(const ReqId &id, const QString &data)
{
    uint16_t req_id = static_cast<uint16_t>(id);

    QByteArray data_bytes = data.toUtf8();

    quint16 data_len = static_cast<quint16>(data_bytes.size());

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setByteOrder(QDataStream::BigEndian);

    out << req_id << data_len;

    block.append(data_bytes);

    socket_->write(block);
}
