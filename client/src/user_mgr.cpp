#include "user_mgr.h"

UserMgr::UserMgr()
    : user_info_(std::make_shared<UserInfo>(0, "", "", "", 0)),
      chat_list_model_(new ChatListModel(this)),
      contact_list_model_(new UserListModel(this)),
      apply_list_model_(new UserListModel(this))
{
}

UserMgr::~UserMgr()
{
}

void UserMgr::setUserInfo(std::shared_ptr<UserInfo> user_info)
{
    user_info_ = user_info;
}

std::shared_ptr<UserInfo> UserMgr::getUserInfo()
{
    return user_info_;
}

void UserMgr::setName(const QString &name)
{
    user_info_->name = name;
}

void UserMgr::setToken(const QString &token)
{
    token_ = token;
}

void UserMgr::setUid(int uid)
{
    user_info_->uid = uid;
}

int UserMgr::getUid()
{
    return user_info_->uid;
}

QString UserMgr::getName()
{
    return user_info_->name;
}

QString UserMgr::getNick()
{
    return user_info_->nick;
}

QString UserMgr::getIcon()
{
    return user_info_->icon;
}

QString UserMgr::getDesc()
{
    return user_info_->desc;
}

ChatListModel *UserMgr::chatListModel() const
{
    return chat_list_model_;
}

UserListModel *UserMgr::contactListModel() const
{
    return contact_list_model_;
}

UserListModel *UserMgr::applyListModel() const
{
    return apply_list_model_;
}

QString UserMgr::getAvatarByUid(int uid) const
{
    return contact_list_model_->getAvatar(uid);
}

bool UserMgr::getOnlineByUid(int uid) const
{
    return contact_list_model_->getOnline(uid);
}

void UserMgr::addApply(std::shared_ptr<AddFriendApply> apply)
{
    auto friend_info = std::make_shared<FriendInfo>(
        apply->fromuid, apply->name, apply->nick, apply->icon,
        apply->sex, apply->desc, "", "");
    apply_list_model_->addUser(friend_info);
}

void UserMgr::removeApply(int uid)
{
    apply_list_model_->removeUser(uid);
}

void UserMgr::addFriend(std::shared_ptr<FriendInfo> friend_info)
{
    if (!contact_list_model_->getName(friend_info->uid).isEmpty())
        return;
    contact_list_model_->addUser(friend_info);
}

void UserMgr::appendApplyList(const QJsonArray &apply_list)
{
    for (const QJsonValue &apply : apply_list)
    {
        auto name = apply["username"].toString();
        auto nick = apply["nick"].toString();
        auto icon = apply["icon"].toString();
        auto uid = apply["uid"].toInt();
        auto sex = apply["sex"].toInt();
        auto desc = apply["desc"].toString();
        auto status = apply["status"].toInt();
        if (status != 0)
        {
            continue;
        }
        auto friend_info = std::make_shared<FriendInfo>(uid, name, nick, icon, sex, desc, "", "");
        apply_list_model_->addUser(friend_info);
    }
}

void UserMgr::appendChatList(const QJsonArray &chat_list)
{
}

void UserMgr::appendContactList(const QJsonArray &contact_list)
{
}

std::shared_ptr<FriendInfo> UserMgr::getFriend(int uid) const
{
    auto it = friend_map_.find(uid);
    if (it != friend_map_.end())
        return it.value();
    return nullptr;
}
