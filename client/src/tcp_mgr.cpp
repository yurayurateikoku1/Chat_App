#include "tcp_mgr.h"
#include <spdlog/spdlog.h>
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include "user_model.h"

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
                                   emit signLoginChatStatus(false);
                                   return;
                               }

                               UserModel::getInstance().setUid(json_obj.value("uid").toInt());
                               UserModel::getInstance().setName(json_obj.value("name").toString().toStdString());
                               UserModel::getInstance().setToken(json_obj.value("token").toString().toStdString());

                               emit signLoginChatStatus(true);
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
