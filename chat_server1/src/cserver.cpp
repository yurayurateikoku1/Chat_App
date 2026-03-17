#include "cserver.h"
#include <spdlog/spdlog.h>
#include "iocontext_pool.h"
CServer::CServer(boost::asio::io_context &io_context, unsigned short port)
    : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), socket_(io_context)
{
}

void CServer::clearSession(const std::string &uid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    session_map_.erase(uid);
}

CServer::~CServer()
{
}

void CServer::handleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code &ec)
{
    if (!ec)
    {
        SPDLOG_INFO("New connection accepted");
        new_session->lunchSession();
        std::lock_guard<std::mutex> lock(mutex_);
        session_map_.insert({new_session->getUid(), new_session});
    }
    else
    {
        SPDLOG_ERROR(ec.message());
    }
    lunchAccept();
}

void CServer::lunchAccept()
{
    auto self = shared_from_this();
    auto &io_context = IOContextPool::getInstance()->getIOContext();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
    acceptor_.async_accept(new_session->getSocket(), [self, new_session](boost::system::error_code ec)
                           { self->handleAccept(new_session, ec); });
}
