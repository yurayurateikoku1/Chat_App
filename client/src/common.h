#pragma once
#include <QString>
enum class ReqId
{
    ID_GET_VERIFY_CODE = 1001,
    ID_REG_USER = 1002
};

enum class Modules
{
    REGISTER = 0,
    LOGIN = 1,
    CHAT = 2
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

inline QString gate_url_prefix;
