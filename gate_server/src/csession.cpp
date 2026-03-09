#include "csession.h"
#include "logic_system.h"
#include "utils.h"
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
        // request_.target() 为HTTP 请求的 URL 路径，
        // 预解析GET参数
        preParseGetParams();
        // shared_from_this()为当前 CSession 的 shared_ptr
        bool result = LogicSystem::getInstance()->handleGetRequest(get_url_, shared_from_this());
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
    case http::verb::post:
    {
        bool result = LogicSystem::getInstance()->handlePostRequest(request_.target(), shared_from_this());
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

void CSession::preParseGetParams()
{
    auto url = request_.target();
    // 查找？的位置
    auto query_pos = url.find('?');
    if (query_pos == std::string::npos)
    {
        get_url_ = url;
        return;
    }

    get_url_ = url.substr(0, query_pos);
    std::string query_string = url.substr(query_pos + 1);
    std::string key, value;
    size_t pos = 0;

    while ((pos = query_string.find("&")) != std::string::npos)
    {
        auto pair = query_string.substr(0, pos);
        size_t pos2 = pair.find("=");
        if (pos2 != std::string::npos)
        {
            key = utils::urlDecode(pair.substr(0, pos2));
            value = utils::urlDecode(pair.substr(pos2 + 1));
            get_params_[key] = value;
        }
        query_string.erase(0, pos + 1);
    }

    // 处理最后一个参数对 如果没有&分隔符
    if (!query_string.empty())
    {
        size_t pos = query_string.find("=");
        if (pos != std::string::npos)
        {
            key = utils::urlDecode(query_string.substr(0, pos));
            value = utils::urlDecode(query_string.substr(pos + 1));
            get_params_[key] = value;
        }
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