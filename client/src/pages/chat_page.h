#pragma once
#include <QObject>
#include <QQmlEngine>
#include "../common.h"
#include "../models/chat_list_model.h"
#include "../models/user_list_model.h"

class ChatPage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(ChatMessageModel *getChatMessageModel READ getChatMessageModel NOTIFY signChatMessageModelChanged)
    Q_PROPERTY(ChatListModel *getChatListModel READ getChatListModel CONSTANT)
    Q_PROPERTY(UserListModel *getContactListModel READ getContactListModel CONSTANT)
    Q_PROPERTY(UserListModel *getSearchListModel READ getSearchListModel CONSTANT)
    Q_PROPERTY(UserListModel *getFriendRequestListModel READ getFriendRequestListModel CONSTANT)

    Q_PROPERTY(int getCurrentUid READ getCurrentUid NOTIFY signCurrentUidChanged)
    Q_PROPERTY(QString getCurrentName READ getCurrentName NOTIFY signCurrentUidChanged)

    // 服务器精确查找用户的结果
    Q_PROPERTY(int foundUid READ foundUid NOTIFY signFoundUserChanged)
    Q_PROPERTY(QString foundName READ foundName NOTIFY signFoundUserChanged)
    Q_PROPERTY(QString foundAvatar READ foundAvatar NOTIFY signFoundUserChanged)
    Q_PROPERTY(bool foundValid READ foundValid NOTIFY signFoundUserChanged)

public:
    explicit ChatPage(QObject *parent = nullptr);

    /// @brief 发送消息
    Q_INVOKABLE void sendMessage(int uid, const QString &message);
    /// @brief 切换当前聊天会话
    Q_INVOKABLE void switchChat(int uid);
    /// @brief 从联系人发起聊天（不存在则创建会话）
    Q_INVOKABLE void startChat(int uid);
    /// @brief 根据uid获取头像路径
    Q_INVOKABLE QString getAvatar(int uid) const;
    /// @brief 查询在线状态
    Q_INVOKABLE bool getOnLine(int uid) const;
    /// @brief 搜索联系人（本地模糊匹配名称）
    Q_INVOKABLE void searchContacts(const QString &keyword);
    /// @brief 通过uid精确查找用户（向服务器查询）
    Q_INVOKABLE void searchUser(int uid);
    /// @brief 清空查找结果
    Q_INVOKABLE void clearFoundUser();
    /// @brief 添加联系人
    Q_INVOKABLE void addUser2Contact(int uid);
    /// @brief 清除好友请求
    Q_INVOKABLE void clearFriendRequest(int uid);

    ChatMessageModel *getChatMessageModel() const;
    ChatListModel *getChatListModel() const;
    UserListModel *getContactListModel() const;
    UserListModel *getSearchListModel() const;
    UserListModel *getFriendRequestListModel() const;
    int getCurrentUid() const;
    QString getCurrentName() const;

    int foundUid() const;
    QString foundName() const;
    QString foundAvatar() const;
    bool foundValid() const;

signals:
    void signChatMessageModelChanged();
    void signCurrentUidChanged();
    void signFoundUserChanged();

private:
    ChatListModel *chat_list_model_;
    UserListModel *contact_list_model_;
    UserListModel *search_list_model_;
    UserListModel *friend_request_list_model_;
    ChatMessageModel *current_message_model_ = nullptr;
    int current_uid_ = -1;

    // 精确查找结果
    int found_uid_ = -1;
    QString found_name_;
    QString found_avatar_;
    bool found_valid_ = false;
};
