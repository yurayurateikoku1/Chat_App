#include "verify_grpc_client.h"

VerifyGrpcClient::VerifyGrpcClient()
{
    std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
    stub_ = VerifyService::NewStub(channel);
}

GetVerifyRsp VerifyGrpcClient::getVerifyCode(const std::string &email)
{
    ClientContext context;
    GetVerifyRsp rsp;
    GetVerifyReq req;
    req.set_email(email);
    Status status = stub_->GetVerifyCode(&context, req, &rsp);

    if (status.ok())
    {
        return rsp;
    }
    else
    {
        rsp.set_error(static_cast<int32_t>(ErrorCode::RPCFAILED));
        return rsp;
    }
}