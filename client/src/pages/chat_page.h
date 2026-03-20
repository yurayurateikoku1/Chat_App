#pragma once
#include <QObject>
#include <QQmlEngine>
#include "../common.h"
#include "../models/user_item.h"
#include "../models/chat_message_model.h"
#include "../models/chat_list_model.h"
#include "../models/user_list_model.h"

class ChatPage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(ChatMessageModel *getChatMessageModel READ getChatMessageModel NOTIFY sign2UIChatMessageModelChanged)
    Q_PROPERTY(ChatListModel *getChatListModel READ getChatListModel CONSTANT)
    Q_PROPERTY(UserListModel *getContactListModel READ getContactListModel CONSTANT)
    Q_PROPERTY(UserListModel *getSearchListModel READ getSearchListModel CONSTANT)
    Q_PROPERTY(UserListModel *getFriendRequestListModel READ getFriendRequestListModel CONSTANT)

    Q_PROPERTY(int getCurrentUid READ getCurrentUid NOTIFY sign2UICurrentUidChanged)
    Q_PROPERTY(QString getCurrentName READ getCurrentName NOTIFY sign2UICurrentUidChanged)
    Q_PROPERTY(QString currentAvatar READ getCurrentAvatar NOTIFY sign2UICurrentUidChanged)

    // 服务器精确查找用户的结果
    Q_PROPERTY(SearchResult getSearchResult READ getSearchResult NOTIFY sign2UISearchResultChanged)

    Q_PROPERTY(QString selfIcon READ getSelfIcon CONSTANT)

    Q_PROPERTY(bool hasFriendRequest READ hasFriendRequest NOTIFY hasFriendRequestChanged)

public:
    explicit ChatPage(QObject *parent = nullptr);

    /// @brief 发送消息
    Q_INVOKABLE void sendMessage(int uid, const QString &message);
    /// @brief 切换当前聊天会话
    Q_INVOKABLE void switchChat(int uid);
    /// @brief 从联系人发起聊天（不存在则创建会话）
    Q_INVOKABLE void startChat(int uid);
    /// @brief 获取自己的头像路径
    Q_INVOKABLE QString getSelfIcon() const;
    /// @brief 根据uid获取头像路径（转发到UserMgr）
    Q_INVOKABLE QString getAvatar(int uid) const;
    /// @brief 查询在线状态（转发到UserMgr）
    Q_INVOKABLE bool getOnLine(int uid) const;
    /// @brief 清除好友请求（转发到UserMgr）
    Q_INVOKABLE void clearFriendRequest(int uid);
    /// @brief 搜索联系人（本地模糊匹配名称）
    Q_INVOKABLE void searchContacts(const QString &keyword);
    /// @brief 通过uid精确查找用户（向服务器查询）
    Q_INVOKABLE void searchUser(int uid);
    /// @brief 清空查找结果
    Q_INVOKABLE void clearSearchResult();
    /// @brief 添加联系人
    Q_INVOKABLE void addUser2Contact(int to_uid);
    /// @brief 认证好友请求
    Q_INVOKABLE void authFriendApply(int to_uid);

    ChatMessageModel *getChatMessageModel() const;
    ChatListModel *getChatListModel() const;
    UserListModel *getContactListModel() const;
    UserListModel *getSearchListModel() const;
    UserListModel *getFriendRequestListModel() const;
    int getCurrentUid() const;
    QString getCurrentName() const;
    QString getCurrentAvatar() const;

    SearchResult getSearchResult() const;
    bool hasFriendRequest() const;
    Q_INVOKABLE void clearFriendRequestBadge();

signals:
    void hasFriendRequestChanged();
    void sign2UIChatMessageModelChanged();
    void sign2UICurrentUidChanged();
    void sign2UISearchResultChanged();
    /// @brief 向UI发送信息
    /// @param message
    /// @param normal
    void sign2UIMessage(const QString &message, bool normal);

private:
    ChatListModel *chat_list_model_;           ///< 聊天会话列表数据模型非拥有指针
    UserListModel *contact_list_model_;        ///< 联系人列表数据模型非拥有指针
    UserListModel *friend_request_list_model_; ///< 好友请求列表数据模型非拥有指针

    ChatMessageModel *current_message_model_ = nullptr; //< 当前聊天会话消息列表数据模型
    int current_chat_uid_ = -1;                         //< 当前聊天会话 uid
    QString current_avatar_;                              //< 当前聊天对方头像

    UserListModel *search_list_model_;                 ///< 搜索结果列表数据模型
    std::shared_ptr<SearchInfo> found_user_ = nullptr; ///< 精确查找到的用户信息
    bool has_friend_request_ = false;                  ///< 是否有未读好友申请
};
