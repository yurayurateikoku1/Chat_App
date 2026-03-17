#include "config_mgr.h"
#include "iocontext_pool.h"
#include "cserver.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

bool flag_stop = false;
std::condition_variable cv;
std::mutex mutex;

void runChatServer()
{
    auto &config_mgr = ConfigMgr::getInstance();
    auto pool = IOContextPool::getInstance();

    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&io_context, pool](boost::system::error_code ec, int signal_number)
                       {if (ec)
                       {
                           return;
                       }
                        pool->stop();
                        io_context.stop(); });
    auto port = static_cast<unsigned short>(std::stoi(config_mgr["chat_server1"]["port"]));
    auto server = std::make_shared<CServer>(io_context, port);
    server->lunchAccept();
    SPDLOG_INFO("Chat server start on port {}", port);
    io_context.run();
}

int main(int argc, char *argv[])
{
    spdlog::stdout_color_mt("console");
    try
    {
        runChatServer();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR(e.what());
        return -1;
    }
    return 0;
}