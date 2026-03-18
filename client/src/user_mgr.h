#pragma once
#include <string>
#include <QObject>
#include "models/user_item.h"

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

    /// @brief 添加申请列表
    /// @param array
    void AppendApplyList(QJsonArray array);

    void AppendFriendList(QJsonArray array);

    std::vector<std::shared_ptr<ApplyInfo>> getApplyList();

    void addApplyList(std::shared_ptr<ApplyInfo> apply_info);

private:
    UserMgr();
    QString token_;
    std::shared_ptr<UserInfo> user_info_;
    std::vector<std::shared_ptr<ApplyInfo>> apply_list_;
    std::vector<std::shared_ptr<FriendInfo>> friend_list_;
    QMap<int, std::shared_ptr<FriendInfo>> friend_map_;
    int chat_loaded_;
    int contact_loaded_;
};