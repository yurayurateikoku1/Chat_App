#pragma once
#include "common.h"
#include "message.grpc.pb.h"
#include "singleton.h"
#include <grpcpp/grpcpp.h>
#include <memory>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

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
    std::unique_ptr<VerifyService::Stub> stub_;
};