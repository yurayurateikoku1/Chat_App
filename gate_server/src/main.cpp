#include "cserver.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
int main()
{
    spdlog::stdout_color_mt("console");
    try
    {
        unsigned short port = 8080;
        net::io_context io_context{1};
        // 注册系统信号处理
        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&io_context](boost::system::error_code ec, int signal_number)
                           {if (ec)
                           {
                               return;
                           }
                            io_context.stop(); });
        std::make_shared<CServer>(io_context, port)->lunchServer();
        io_context.run();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR(e.what());
    }
}