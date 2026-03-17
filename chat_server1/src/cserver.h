#pragma once
#include "common.h"
#include "csession.h"
#include <map>
class CServer : public std::enable_shared_from_this<CServer>
{

public:
    CServer(boost::asio::io_context &io_context, unsigned short port);

    /// @brief 清除会话
    void clearSession(const std::string &uid);
    ~CServer();
    /// @brief 启动接受
    void lunchAccept();

private:
    /// @brief 处理接受
    /// @param new_session
    /// @param ec
    void handleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code &ec);

    std::map<std::string, std::shared_ptr<CSession>> session_map_;
    std::mutex mutex_;
    short port_;
    boost::asio::ip::tcp::acceptor acceptor_; // 接受器
    net::io_context &io_context_;             // 网络上下文
    boost::asio::ip::tcp::socket socket_;     // 套接字，用于接受客户端连接
};
