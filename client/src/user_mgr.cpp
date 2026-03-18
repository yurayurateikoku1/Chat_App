#include "user_mgr.h"

UserMgr::UserMgr()
    : user_info_(std::make_shared<UserInfo>(0, "", "", "", 0))
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
