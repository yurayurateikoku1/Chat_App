#pragma once
#include <string>
class UserModel
{
public:
    static UserModel &getInstance()
    {
        static UserModel instance;
        return instance;
    }
    ~UserModel();

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
    UserModel();
    std::string name_;
    std::string token_;
    int uid_;
};