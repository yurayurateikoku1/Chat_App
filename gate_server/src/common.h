#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>

#define CODEPREFIX "code_"

#define MAX_LENGTH 1024 * 2 // 最大长度

#define HEAD_TOTAL_LEN 4 // 头部总长度

#define HEAD_ID_LEN 2 // 头部id长度

#define HEAD_DATA_LEN 2 // 头部数据长度

#define MAX_RECVQUE 10000 // 最大接收队列

#define MAX_SENDQUE 1000 // 最大发送队列

#define USERIPPREFIX "uip_"

#define USERTOKENPREFIX "utoken_"

#define IPCOUNTPREFIX "ipcount_"

#define USER_BASE_INFO "ubaseinfo_"

#define LOGIN_COUNT "logincount"

#define NAME_INFO "nameinfo_"

#define LOCK_PREFIX "lock_"

#define USER_SESSION_PREFIX "usession_"

#define LOCK_COUNT "lockcount"

enum class MSG_IDS
{
    MSG_CHAT_LOGIN = 1005,              // 用户登陆
    MSG_CHAT_LOGIN_RSP = 1006,          // 用户登陆回包
    ID_SEARCH_USER_REQ = 1007,          // 用户搜索请求
    ID_SEARCH_USER_RSP = 1008,          // 搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,           // 申请添加好友请求
    ID_ADD_FRIEND_RSP = 1010,           // 申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,    // 通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,          // 认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,          // 认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015,   // 通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ = 1017,        // 文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP = 1018,        // 文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 通知用户文本聊天信息
    ID_NOTIFY_OFF_LINE_REQ = 1021,      // 通知用户下线
    ID_HEART_BEAT_REQ = 1023,           // 心跳请求
    ID_HEARTBEAT_RSP = 1024,            // 心跳回复
};

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

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

class Defer
{
public:
    Defer(std::function<void()> func) : func_(func) {}
    ~Defer() { func_(); }

private:
    std::function<void()> func_;
};
