#pragma once
#include "common.h"
class CServer : public std::enable_shared_from_this<CServer>
{

public:
    CServer(boost::asio::io_context &io_context, unsigned short port);

    /// @brief 开启服务
    void lunchServer();

    ~CServer();

private:
    boost::asio::ip::tcp::acceptor acceptor_; // 接受器
    net::io_context &io_context_;             // 网络上下文
    boost::asio::ip::tcp::socket socket_;     // 套接字，用于接受客户端连接
};
