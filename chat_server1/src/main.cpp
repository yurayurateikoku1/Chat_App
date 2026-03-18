#include "config_mgr.h"
#include "common.h"
#include "iocontext_pool.h"
#include "cserver.h"
#include "redis_mgr.h"
#include "chat_service_impl.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

bool flag_stop = false;
std::condition_variable cv;
std::mutex mutex;

void runChatServer()
{
    auto &config_mgr = ConfigMgr::getInstance();
    auto server_name = config_mgr["self_server"]["name"];

    auto pool = IOContextPool::getInstance();

    RedisMgr::getInstance().hsetValue(LOGIN_COUNT, server_name, "0");

    std::string server_address(config_mgr["self_server"]["host"] + ":" + config_mgr["self_server"]["rpc_port"]);

    // 创建grpc服务
    ChatServiceImpl service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> grpc_server(builder.BuildAndStart());
    SPDLOG_INFO("RPC server listening on {}", server_address);
    std::thread grpc_server_thread([&grpc_server]
                                   { grpc_server->Wait(); });

    boost::asio::io_context io_context;
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&io_context, pool](boost::system::error_code ec, int signal_number)
                       {if (ec)
                       {
                           return;
                       }
                        pool->stop();
                        io_context.stop(); });
    auto port = static_cast<unsigned short>(std::stoi(config_mgr["self_server"]["port"]));
    auto server = std::make_shared<CServer>(io_context, port);
    server->lunchAccept();
    SPDLOG_INFO("Chat server start on port {}", port);
    io_context.run();

    RedisMgr::getInstance().hdelValue(LOGIN_COUNT, server_name);

    grpc_server_thread.join();
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