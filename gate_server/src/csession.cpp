#include "csession.h"
#include "logic_system.h"
#include <spdlog/spdlog.h>
CSession::CSession(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket))
{
}

void CSession::lunchSession()
{
    auto self = shared_from_this();
    http::async_read(socket_, buffer_, request_, [self](boost::system::error_code ec, std::size_t bytes_transferred)
                     {
                         try
                         {
                             if (ec)
                             {
                                SPDLOG_ERROR(ec.message());
                                return;
                             }
                             boost::ignore_unused(bytes_transferred);//告诉编译器这个变量没用到，不要我警告
                             self->readRequest();
                             //超时检测
                             self->checkDeadline();
                         }
                         catch (const std::exception &e)
                         {
                             SPDLOG_ERROR(e.what());
                         } });
}

void CSession::readRequest()
{
    // 设置版本
    response_.version(request_.version());
    response_.keep_alive(false);

    switch (request_.method())
    {
    case http::verb::get:
    {
        // request_.target() 为HTTP 请求的 URL 路径，shared_from_this()为当前 CSession 的 shared_ptr
        bool result = LogicSystem::getInstance()->handleGetRequest(request_.target(), shared_from_this());
        if (!result)
        {
            response_.result(http::status::not_found); // 404
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "Url not Found\r\n";
            wirteResponse();
            break;
        }

        response_.result(http::status::ok); // 200
        response_.set(http::field::server, "gate_server");
        wirteResponse();
        break;
    }
    default:
        break;
    }
}

void CSession::checkDeadline()
{
    auto self = shared_from_this();
    deadline_.async_wait([self](boost::system::error_code ec)
                         {
                             if (!ec)
                             {
                                self->socket_.close(ec);
                             } });
}

void CSession::wirteResponse()
{
    auto self = shared_from_this();
    response_.content_length(response_.body().size());
    http::async_write(socket_, response_, [self](boost::system::error_code ec, std::size_t bytes_transferred)
                      {
                        self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                        self->deadline_.cancel(); });
}