#pragma once
#include "common.h"
#include "config_mgr.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <queue>
#include <memory>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

/// @brief 状态grpc连接池
class StatusConnectionPool
{
public:
    StatusConnectionPool(size_t pool_size, const std::string &host, const std::string &port);
    ~StatusConnectionPool();
    std::unique_ptr<StatusService::Stub> getConnection();
    void returnConnection(std::unique_ptr<StatusService::Stub> stub);

private:
    std::size_t pool_size_;
    std::string host_;
    std::string port_;
    std::atomic<bool> flag_stop_;
    std::queue<std::unique_ptr<StatusService::Stub>> stubs_;
    std::condition_variable cv_;
    std::mutex mutex_;
};

/// @brief 状态grpc客户端
class StatusGrpcClient
{
public:
    static StatusGrpcClient &getInstance()
    {
        static StatusGrpcClient instance;
        return instance;
    }

    ~StatusGrpcClient()
    {
    }
    GetChatServerRsp getChatServer(int uid);
    LoginRsp login(int uid, const std::string &token);

private:
    StatusGrpcClient();
    std::unique_ptr<StatusConnectionPool> status_pool_;
};