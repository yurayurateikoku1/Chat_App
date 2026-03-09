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
    ERROR_JSON = 1001,
    RPCFAILED = 1002
};