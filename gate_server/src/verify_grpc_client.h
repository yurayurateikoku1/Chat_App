#pragma once
#include "common.h"
#include "message.grpc.pb.h"
#include "singleton.h"
#include <grpcpp/grpcpp.h>
#include <memory>
#include <atomic>
#include <queue>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

/// @brief rpc连接池
class RPCConnectionPool
{
public:
    RPCConnectionPool(std::size_t size, const std::string &host, const std::string &port);
    ~RPCConnectionPool();
    std::unique_ptr<VerifyService::Stub> getStub();
    void returnStub(std::unique_ptr<VerifyService::Stub> stub);

private:
    std::atomic<bool> flag_stop_;
    std::size_t size_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<VerifyService::Stub>> stubs_;
    std::condition_variable cv_;
    std::mutex mutex_;
};

/// @brief 验证码grpc客户端
class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;

public:
    /// @brief 获取验证码
    /// @param email
    /// @return
    GetVerifyRsp getVerifyCode(const std::string &email);

private:
    VerifyGrpcClient();
    std::unique_ptr<RPCConnectionPool> rpc_pool_;
};