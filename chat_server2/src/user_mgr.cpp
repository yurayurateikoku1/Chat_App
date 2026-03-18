#include "user_mgr.h"
#include "csession.h"
#include "redis_mgr.h"

UserMgr::UserMgr()
{
}

UserMgr::~UserMgr()
{
    uid2sessione_map_.clear();
}

std::shared_ptr<CSession> UserMgr::getCSeSsion(int uid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = uid2sessione_map_.find(uid);
    if (it != uid2sessione_map_.end())
    {
        return it->second;
    }
    return nullptr;
}

void UserMgr::setUserSession(int uid, std::shared_ptr<CSession> session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    uid2sessione_map_[uid] = session;
}

void UserMgr::removeUserSession(int uid)
{
    auto uid_str = std::to_string(uid);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        uid2sessione_map_.erase(uid);
    }
}
