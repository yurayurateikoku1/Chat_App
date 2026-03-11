#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>

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

};

#define CODEPREFIX "code_"

class Defer
{
public:
    Defer(std::function<void()> func) : func_(func) {}
    ~Defer() { func_(); }

private:
    std::function<void()> func_;
};
