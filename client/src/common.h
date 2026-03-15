#pragma once
#include <QString>
enum class ReqId
{
    ID_GET_VERIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002,        // 注册用户
    ID_RESET_PASSWORD = 1003,  // 重置密码
    ID_LOGIN_USER = 1004,      // 用户登录
    ID_CHAT_LOGIN = 1005,      // 聊天登录
    ID_CHAT_LOGIN_RSP = 1006   // 聊天登录响应

};

enum class Modules
{
    REGISTER = 0, // 注册
    RESET = 1,    // 重置
    LOGIN = 2     // 登录
};

enum class ErrorCode
{
    SUCCESS = 0,
    ERROR_JSON = 1001,       // json解析失败
    RPCFAILED = 1002,        // rpc调用失败
    VERIFYEXPIRED = 1003,    // 验证码过期
    ERROR_VERIFYCODE = 1004, // 验证码错误
    USEREXISTED = 1005,      // 用户已存在
    ERROR_PASSWD = 1006,     // 密码错误
    EMAILNOTMATCH = 1007,    // 邮箱不匹配
    PASSWDUPFAILED = 1008,   // 密码修改失败
};

struct ServerInfo
{
    std::string host;
    std::string port;
    std::string token;
    int uid;
};

inline QString gate_url_prefix;
