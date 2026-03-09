#pragma once
#include "common.h"

/// @brief 会话
class CSession : public std::enable_shared_from_this<CSession>
{
public:
    friend class LogicSystem;
    CSession(boost::asio::ip::tcp::socket socket);
    /// @brief 启动会话
    void lunchSession();

private:
    /// @brief 超时检测
    void checkDeadline();

    /// @brief 发送响应
    /// @param res
    void wirteResponse();

    /// @brief 处理请求
    /// @param req
    void readRequest();

    /// @brief 预解析GET参数
    void preParseGetParams();

    boost::asio::ip::tcp::socket socket_;         // 套接字，用于发送和接受数据
    beast::flat_buffer buffer_{8192};             // 缓冲区
    http::request<http::dynamic_body> request_;   // 请求对象
    http::response<http::dynamic_body> response_; // 响应对象

    /// @brief 超时检测
    net::steady_timer deadline_{
        socket_.get_executor(), std::chrono::seconds(60)}; // 超时时间

    std::string get_url_;                                     // GET请求的URL
    std::unordered_map<std::string, std::string> get_params_; // GET请求的参数
};