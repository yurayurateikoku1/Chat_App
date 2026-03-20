#include "chat_page.h"
#include "../user_mgr.h"
#include "../tcp_mgr.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <spdlog/spdlog.h>

ChatPage::ChatPage(QObject *parent)
    : QObject(parent),
      chat_list_model_(UserMgr::getInstance().chatListModel()),
      contact_list_model_(UserMgr::getInstance().contactListModel()),
      search_list_model_(new UserListModel(this)),
      friend_request_list_model_(UserMgr::getInstance().applyListModel())
{
    connect(TCPMgr::getInstance().get(), &TCPMgr::signSearchUserResult, this, [this](std::shared_ptr<SearchInfo> info)
            {
        found_user_ = info;
        emit sign2UISearchResultChanged(); });

    // 在线收到好友申请 → 存入 UserMgr，model 自动更新 UI，显示小红点
    connect(TCPMgr::getInstance().get(), &TCPMgr::signReceiveFriendApply, this, [this](std::shared_ptr<AddFriendApply> info)
            {
        UserMgr::getInstance().addApply(info);
        if (!has_friend_request_) {
            has_friend_request_ = true;
            emit hasFriendRequestChanged();
        } });

    // 登录时已加载的申请列表，初始化小红点状态
    if (friend_request_list_model_ && friend_request_list_model_->rowCount() > 0)
    {
        has_friend_request_ = true;
        emit hasFriendRequestChanged();
    }

    // 接收AI聊天回复
    connect(TCPMgr::getInstance().get(), &TCPMgr::signAIChatResponse, this, [this](int from_uid, const QString &message)
            {
        // AI的uid固定为0
        constexpr int AI_UID = 0;
        auto *model = chat_list_model_->getMessageModel(AI_UID);
        if (model)
        {
            auto now = QDateTime::currentDateTime();
            QString time_str = now.toString("hh:mm");
            QString msgid = QUuid::createUuid().toString();
            model->addMessage(msgid, message, time_str, false);
        }

        // 记录到历史
        QJsonObject history_item;
        history_item["role"] = "assistant";
        history_item["content"] = message;
        ai_chat_history_.append(history_item); });
}

Q_INVOKABLE void ChatPage::sendMessage(int uid, const QString &message)
{
    if (message.isEmpty())
        return;

    auto *model = chat_list_model_->getMessageModel(uid);
    if (!model)
        return;

    auto now = QDateTime::currentDateTime();
    QString time_str = now.toString("hh:mm");
    QString id = QUuid::createUuid().toString();

    // 先把用户消息显示到界面
    model->addMessage(id, message, time_str, true);

    if (uid == 0)
    {
        // AI聊天通道
        QJsonObject history_item;
        history_item["role"] = "user";
        history_item["content"] = message;
        ai_chat_history_.append(history_item);

        QJsonObject json;
        json["from_uid"] = UserMgr::getInstance().getUid();
        json["message"] = message;
        json["history"] = ai_chat_history_;

        QJsonDocument doc(json);
        QByteArray data = doc.toJson(QJsonDocument::Compact);
        QMetaObject::invokeMethod(TCPMgr::getInstance().get(), [data]()
                                  { TCPMgr::getInstance()->tcpSendData(ReqId::ID_AI_CHAT_REQ, data); }, Qt::QueuedConnection);
    }
    else
    {
        // 普通聊天通道
        QJsonObject json;
        QJsonObject msg;
        msg["msgid"] = id;
        msg["content"] = message;
        QJsonArray textArray;
        textArray.append(msg);
        json["from_uid"] = UserMgr::getInstance().getUid();
        json["to_uid"] = uid;
        json["text_array"] = textArray;

        QJsonDocument doc(json);
        QByteArray data = doc.toJson(QJsonDocument::Compact);
        QMetaObject::invokeMethod(TCPMgr::getInstance().get(), [data]()
                                  { TCPMgr::getInstance()->tcpSendData(ReqId::ID_TEXT_CHAT_MSG_REQ, data); }, Qt::QueuedConnection);
    }
}

Q_INVOKABLE void ChatPage::switchChat(int uid)
{
    if (current_chat_uid_ == uid)
        return;

    current_chat_uid_ = uid;
    current_message_model_ = chat_list_model_->getMessageModel(uid);
    current_avatar_ = UserMgr::getInstance().getAvatarByUid(uid);
    chat_list_model_->setCurrentChatUid(uid);
    chat_list_model_->clearUnread(uid);

    emit sign2UICurrentUidChanged();
    emit sign2UIChatMessageModelChanged();
}

Q_INVOKABLE void ChatPage::startChat(int uid)
{
    if (!chat_list_model_->hasChat(uid))
    {
        auto &mgr = UserMgr::getInstance();
        QString name = mgr.contactListModel()->getName(uid);
        QString avatar = mgr.getAvatarByUid(uid);
        auto info = std::make_shared<FriendInfo>(uid, name, name, avatar, 0, "", "", "");
        chat_list_model_->addChat(info);
    }
    switchChat(uid);
}

Q_INVOKABLE QString ChatPage::getSelfIcon() const
{
    return UserMgr::getInstance().getIcon();
}

Q_INVOKABLE QString ChatPage::getAvatar(int uid) const
{
    return UserMgr::getInstance().getAvatarByUid(uid);
}

Q_INVOKABLE bool ChatPage::getOnLine(int uid) const
{
    return UserMgr::getInstance().getOnlineByUid(uid);
}

Q_INVOKABLE void ChatPage::clearFriendRequest(int uid)
{
    UserMgr::getInstance().removeApply(uid);
}

Q_INVOKABLE void ChatPage::searchContacts(const QString &keyword)
{
    search_list_model_->clearUsers();

    if (keyword.isEmpty())
        return;

    const auto &users = contact_list_model_->getUsers();
    for (const auto &user : users)
    {
        if (user->name.contains(keyword, Qt::CaseInsensitive))
        {
            search_list_model_->addUser(user);
        }
    }
}

Q_INVOKABLE void ChatPage::searchUser(int uid)
{
    clearSearchResult();
    QJsonObject json;
    json["uid"] = uid;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    QMetaObject::invokeMethod(TCPMgr::getInstance().get(), [data]
                              { TCPMgr::getInstance()->tcpSendData(ReqId::ID_SEARCH_USER_REQ, data); }, Qt::QueuedConnection);

    sign2UIMessage("Searching user...", true);
}

Q_INVOKABLE void ChatPage::clearSearchResult()
{
    found_user_ = nullptr;
    emit sign2UISearchResultChanged();
}

Q_INVOKABLE void ChatPage::addUser2Contact(int to_uid)
{
    QJsonObject json;
    auto from_uid = UserMgr::getInstance().getUid();
    json["uid"] = from_uid;
    json["to_uid"] = to_uid;
    json["apply_name"] = UserMgr::getInstance().getName();
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    QMetaObject::invokeMethod(TCPMgr::getInstance().get(), [data]
                              { TCPMgr::getInstance()->tcpSendData(ReqId::ID_ADD_FRIEND_REQ, data); }, Qt::QueuedConnection);
    sign2UIMessage("Sending add friend request...", true);
    SPDLOG_INFO("uid {} send add friend request to to_uid {}", from_uid, to_uid);
}

Q_INVOKABLE void ChatPage::authFriendApply(int to_uid)
{
    QJsonObject json;
    auto from_uid = UserMgr::getInstance().getUid();
    json["from_uid"] = from_uid;
    json["to_uid"] = to_uid;
    json["back"] = UserMgr::getInstance().getUserInfo()->nick;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    QMetaObject::invokeMethod(TCPMgr::getInstance().get(), [data]
                              { TCPMgr::getInstance()->tcpSendData(ReqId::ID_AUTH_FRIEND_REQ, data); }, Qt::QueuedConnection);
    SPDLOG_INFO("from_uid {} send authFriendApply request to to_uid {}", from_uid, to_uid);
    emit sign2UIMessage("Accepting friend request...", true);
}

ChatMessageModel *ChatPage::getChatMessageModel() const
{
    return current_message_model_;
}

ChatListModel *ChatPage::getChatListModel() const
{
    return chat_list_model_;
}

UserListModel *ChatPage::getContactListModel() const
{
    return contact_list_model_;
}

UserListModel *ChatPage::getSearchListModel() const
{
    return search_list_model_;
}

UserListModel *ChatPage::getFriendRequestListModel() const
{
    return friend_request_list_model_;
}

int ChatPage::getCurrentUid() const
{
    return current_chat_uid_;
}

QString ChatPage::getCurrentName() const
{
    return chat_list_model_->getNameByUid(current_chat_uid_);
}

QString ChatPage::getCurrentAvatar() const
{
    return current_avatar_;
}

SearchResult ChatPage::getSearchResult() const
{
    if (!found_user_)
        return {};
    return {found_user_->uid, found_user_->name, found_user_->icon, true};
}

bool ChatPage::hasFriendRequest() const
{
    return has_friend_request_;
}

void ChatPage::clearFriendRequestBadge()
{
    if (has_friend_request_)
    {
        has_friend_request_ = false;
        emit hasFriendRequestChanged();
    }
}
