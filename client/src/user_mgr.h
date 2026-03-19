#pragma once
#include <string>
#include <QObject>
#include "models/user_item.h"
#include "models/user_list_model.h"
#include "models/chat_list_model.h"

class UserMgr : public QObject
{
    Q_OBJECT
public:
    static UserMgr &getInstance()
    {
        static UserMgr instance;
        return instance;
    }
    ~UserMgr();

    void setUserInfo(std::shared_ptr<UserInfo> user_info);
    std::shared_ptr<UserInfo> getUserInfo();

    void setName(const QString &name);
    void setToken(const QString &token);
    void setUid(int uid);

    int getUid();
    QString getName();
    QString getNick();
    QString getIcon();
    QString getDesc();

    /// @brief 聊天会话列表 model
    ChatListModel *chatListModel() const;

    /// @brief 好友列表 model
    UserListModel *contactListModel() const;

    /// @brief 好友申请列表 model
    UserListModel *applyListModel() const;

    /// @brief 根据uid获取头像路径
    QString getAvatarByUid(int uid) const;

    /// @brief 根据uid查询在线状态
    bool getOnlineByUid(int uid) const;

    /// @brief 添加一条好友申请
    void addApply(std::shared_ptr<AddFriendApply> apply);

    /// @brief 移除一条好友申请
    void removeApply(int uid);

    /// @brief 添加一个好友
    void addFriend(std::shared_ptr<FriendInfo> friend_info);

    /// @brief 根据uid查好友
    std::shared_ptr<FriendInfo> getFriend(int uid) const;

private:
    UserMgr();
    QString token_;
    std::shared_ptr<UserInfo> user_info_; // 个人信息
    ChatListModel *chat_list_model_;      // 聊天会话
    UserListModel *contact_list_model_;   // 好友
    UserListModel *apply_list_model_;     // 好友申请
    QMap<int, std::shared_ptr<FriendInfo>> friend_map_;
};
