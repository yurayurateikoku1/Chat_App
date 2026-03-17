#include "chat_page.h"

ChatPage::ChatPage(QObject *parent)
    : QObject(parent),
      chat_list_model_(new ChatListModel(this)),
      contact_list_model_(new UserListModel(this)),
      search_list_model_(new UserListModel(this)),
      friend_request_list_model_(new UserListModel(this))
{
    // 测试数据 - 联系人
    contact_list_model_->addUser(1, "小明", "qrc:/assets/avatars/Artboard_1.png", true, true);
    contact_list_model_->addUser(2, "小红", "qrc:/assets/avatars/Artboard_2.png", false, true);
    contact_list_model_->addUser(3, "张三", "qrc:/assets/avatars/Artboard_3.png", true, true);
    contact_list_model_->addUser(4, "李四", "qrc:/assets/avatars/Artboard_4.png", false, true);
    contact_list_model_->addUser(5, "王五", "qrc:/assets/avatars/Artboard_5.png", true, true);
    contact_list_model_->addUser(6, "赵六", "qrc:/assets/avatars/Artboard_6.png", false, true);
    contact_list_model_->addUser(7, "孙七", "qrc:/assets/avatars/Artboard_7.png", true, true);
    contact_list_model_->addUser(8, "周八", "qrc:/assets/avatars/Artboard_8.png", false, true);

    // 测试数据 - 好友请求
    friend_request_list_model_->addUser(9, "刘九", "qrc:/assets/avatars/Artboard_1.png", true, false);
    friend_request_list_model_->addUser(10, "吴十", "qrc:/assets/avatars/Artboard_2.png", false, false);
    friend_request_list_model_->addUser(11, "陈十一", "", false, false);

    // 测试数据 - 会话及消息（头像从联系人模型自动查询）
    chat_list_model_->addChat(1, "小明", "在吗？", "10:30", contact_list_model_->getAvatar(1), 2);
    chat_list_model_->addChat(3, "张三", "明天见", "09:15", contact_list_model_->getAvatar(3), 0);
    chat_list_model_->addChat(5, "王五", "收到，谢谢", "昨天", contact_list_model_->getAvatar(5), 5);

    // 小明的聊天记录
    auto *msg1 = chat_list_model_->getMessageModel(1);
    msg1->addMessage("你好呀", "10:00", false);
    msg1->addMessage("你好！", "10:01", true);
    msg1->addMessage("最近怎么样？", "10:05", false);
    msg1->addMessage("挺好的，你呢？", "10:06", true);
    msg1->addMessage("在吗？", "10:30", false);

    // 张三的聊天记录
    auto *msg3 = chat_list_model_->getMessageModel(3);
    msg3->addMessage("明天有空吗？", "09:10", true);
    msg3->addMessage("有空，怎么了？", "09:12", false);
    msg3->addMessage("一起吃饭吧", "09:13", true);
    msg3->addMessage("明天见", "09:15", false);

    // 王五的聊天记录
    auto *msg5 = chat_list_model_->getMessageModel(5);
    msg5->addMessage("文件发你了", "昨天", true);
    msg5->addMessage("收到，谢谢", "昨天", false);
}

Q_INVOKABLE void ChatPage::sendMessage(int uid, const QString &message)
{
    if (message.isEmpty())
        return;

    auto *model = chat_list_model_->getMessageModel(uid);
    if (!model)
        return;
    // TODO: 替换为实际的时间获取
    model->addMessage(message, "10:10", true);
}

Q_INVOKABLE void ChatPage::switchChat(int uid)
{
    if (current_uid_ == uid)
        return;

    current_uid_ = uid;
    current_message_model_ = chat_list_model_->getMessageModel(uid);
    chat_list_model_->clearUnread(uid);

    emit signCurrentUidChanged();
    emit signChatMessageModelChanged();
}

Q_INVOKABLE void ChatPage::startChat(int uid)
{
    if (!chat_list_model_->hasChat(uid))
    {
        QString name = contact_list_model_->getName(uid);
        QString avatar = contact_list_model_->getAvatar(uid);
        chat_list_model_->addChat(uid, name, "", "", avatar, 0);
    }
    switchChat(uid);
}

Q_INVOKABLE QString ChatPage::getAvatar(int uid) const
{
    return contact_list_model_->getAvatar(uid);
}

Q_INVOKABLE bool ChatPage::getOnLine(int uid) const
{
    return contact_list_model_->getOnline(uid);
}

Q_INVOKABLE void ChatPage::searchContacts(const QString &keyword)
{
    search_list_model_->clearUsers();

    if (keyword.isEmpty())
        return;

    const auto &users = contact_list_model_->getUsers();
    for (const auto &user : users)
    {
        if (user.name.contains(keyword, Qt::CaseInsensitive))
        {
            search_list_model_->addUser(user.uid, user.name, user.avatar_source,
                                        user.online, user.is_friend);
        }
    }
}

Q_INVOKABLE void ChatPage::searchUser(int uid)
{
    clearFoundUser();

    // TODO: 替换为实际的服务器请求
    // 目前用本地联系人数据模拟
    const auto &users = contact_list_model_->getUsers();
    for (const auto &user : users)
    {
        if (user.uid == uid)
        {
            found_uid_ = user.uid;
            found_name_ = user.name;
            found_avatar_ = user.avatar_source;
            found_valid_ = true;
            emit signFoundUserChanged();
            return;
        }
    }
    // 未找到，found_valid_ 保持 false
    emit signFoundUserChanged();
}

Q_INVOKABLE void ChatPage::clearFoundUser()
{
    found_uid_ = -1;
    found_name_.clear();
    found_avatar_.clear();
    found_valid_ = false;
    emit signFoundUserChanged();
}

Q_INVOKABLE void ChatPage::addUser2Contact(int uid)
{
    // TODO: 替换为实际的服务器请求
}

Q_INVOKABLE void ChatPage::clearFriendRequest(int uid)
{
    friend_request_list_model_->removeUser(uid);
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
    return current_uid_;
}

QString ChatPage::getCurrentName() const
{
    return chat_list_model_->getNameByUid(current_uid_);
}

int ChatPage::foundUid() const { return found_uid_; }
QString ChatPage::foundName() const { return found_name_; }
QString ChatPage::foundAvatar() const { return found_avatar_; }
bool ChatPage::foundValid() const { return found_valid_; }
