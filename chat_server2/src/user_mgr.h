#pragma once
#include <memory>
#include <map>
#include <mutex>
class CSession;
class UserMgr
{
public:
    static UserMgr &getInstance()
    {
        static UserMgr instance;
        return instance;
    }
    UserMgr(UserMgr const &) = delete;
    void operator=(UserMgr const &) = delete;
    ~UserMgr();

    std::shared_ptr<CSession> getCSeSsion(int uid);
    void setUserSession(int uid, std::shared_ptr<CSession> session);
    void removeUserSession(int uid);

private:
    UserMgr();
    std::mutex mutex_;
    std::map<int, std::shared_ptr<CSession>> uid2sessione_map_;
};