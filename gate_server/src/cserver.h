#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class CServer : public std::enable_shared_from_this<CServer>
{

public:
    CServer(boost::asio::io_context &io_context, unsigned short port);

    void startServer();

    ~CServer();

private:
    boost::asio::ip::tcp::acceptor acceptor_; // 接受器
    net::io_context &io_context_;             // 网络上下文
    boost::asio::ip::tcp::socket socket_;     // 套接字
};
