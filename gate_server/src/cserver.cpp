#include "cserver.h"
#include "csession.h"
#include <spdlog/spdlog.h>
#include "iocontext_pool.h"
CServer::CServer(boost::asio::io_context &io_context, unsigned short port)
    : io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), socket_(io_context)
{
}

void CServer::lunchServer()
{
    auto self = shared_from_this();
    // 获取 io_context
    auto &io_context = IOContextPool::getInstance()->getIOContext();
    std::shared_ptr<CSession> new_context = std::make_shared<CSession>(io_context);
    // 开始异步接受
    acceptor_.async_accept(new_context->getSocket(), [self, new_context](boost::system::error_code ec)
                           {
                               try
                               {
                                    if (ec)
                                    {
                                        // 接受失败,继续接受
                                        self->lunchServer();
                                        return;
                                    }
                                    // 接受成功,启动
                                    new_context->lunchSession();
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
