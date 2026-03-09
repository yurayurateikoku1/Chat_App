#include "cserver.h"
#include "csession.h"
#include <spdlog/spdlog.h>
CServer::CServer(boost::asio::io_context &io_context, unsigned short port)
    : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), socket_(io_context)
{
}

void CServer::lunchServer()
{
    auto self = shared_from_this();
    // 开始异步接受
    acceptor_.async_accept(socket_, [self](boost::system::error_code ec)
                           {
                               try
                               {
                                    if (ec)
                                    {
                                        // 接受失败,继续接受
                                        self->lunchServer();
                                        return;
                                    }
                                    // 接受成功,创建会话并启动
                                    std::make_shared<CSession>(std::move(self->socket_))->lunchSession();
                                    // 继续接受下一个连接
                                    self->lunchServer();
                               }
                               catch (const std::exception &e)
                               {
                                    SPDLOG_ERROR(e.what());
                               } });
}

CServer::~CServer()
{
}
