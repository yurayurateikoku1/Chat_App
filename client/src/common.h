#pragma once
#include <QString>
enum class ReqId
{
    ID_GET_VERIFY_CODE = 1001,          // 获取验证码
    ID_REG_USER = 1002,                 // 注册用户
    ID_RESET_PASSWORD = 1003,           // 重置密码
    ID_LOGIN_USER = 1004,               // 用户登录
    ID_CHAT_LOGIN = 1005,               // 聊天登录
    ID_CHAT_LOGIN_RSP = 1006,           // 聊天登录响应
    ID_SEARCH_USER_REQ = 1007,          // 用户搜索请求
    ID_SEARCH_USER_RSP = 1008,          // 搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,           // 添加好友申请
    ID_ADD_FRIEND_RSP = 1010,           // 申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,    // 通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,          // 认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,          // 认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015,   // 通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ = 1017,        // 文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP = 1018,        // 文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 通知用户文本聊天信息

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
    TOKENINVALID = 1009,     // token无效
    UIDINVALID = 1010,       // uid无效
};

struct ServerInfo
{
    std::string host;
    std::string port;
    std::string token;
    int uid;
};

inline QString gate_url_prefix;
