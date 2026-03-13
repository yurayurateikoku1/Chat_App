#include "config_mgr.h"
#include "status_service_impl.h"
#include "redis_mgr.h"
#include "mysql_mgr.h"
#include "common.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <boost/asio.hpp>
#include "iocontext_pool.h"
#include <nlohmann/json.hpp>

void runStatusServer()
{
    auto &config_mgr = ConfigMgr::getInstance();

    std::string address(config_mgr["status_server"]["host"] + ":" + config_mgr["status_server"]["port"]);
    StatusServiceImpl service;

    grpc::ServerBuilder builder;
    // 添加端口和监听服务
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    // 创建服务
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    SPDLOG_INFO("Status server listening on {}", address);

    // 创建Boost.Asio的io_context对象
    boost::asio::io_context io_context;
    // 创建signal_set对象
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    // 注册信号处理函数
    signals.async_wait([&server, &io_context](boost::system::error_code ec, int signal_number)
                       {if (!ec)
                       {

                          server->Shutdown();
                          io_context.stop(); 
                       } });
    std::thread([&io_context]()
                { io_context.run(); })
        .detach();
    server->Wait();
}

int main(int argc, char *argv[])
{
    spdlog::stdout_color_mt("console");
    try
    {
        runStatusServer();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR(e.what());
        return -1;
    }
    return 0;
}