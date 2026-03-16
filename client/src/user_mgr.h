#pragma once
#include <string>
class UserMgr
{
public:
    static UserMgr &getInstance()
    {
        static UserMgr instance;
        return instance;
    }
    ~UserMgr();

    void setName(const std::string &name)
    {
        name_ = name;
    }

    void setToken(const std::string &token)
    {
        token_ = token;
    }

    void setUid(int uid)
    {
        uid_ = uid;
    }

private:
    UserMgr();
    std::string name_;
    std::string token_;
    int uid_;
};