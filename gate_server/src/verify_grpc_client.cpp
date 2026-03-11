#include "verify_grpc_client.h"
#include "config_mgr.h"

VerifyGrpcClient::VerifyGrpcClient()
{
    auto &config_mgr = ConfigMgr::getInstance();
    std::string host = config_mgr["verify_server"]["host"];
    std::string port = config_mgr["verify_server"]["port"];
    rpc_pool_.reset(new RPCConnectionPool(5, host, port));
}

GetVerifyRsp VerifyGrpcClient::getVerifyCode(const std::string &email)
{
    ClientContext context;
    GetVerifyRsp rsp;
    GetVerifyReq req;
    req.set_email(email);
    auto stub = rpc_pool_->getStub();
    Status status = stub->GetVerifyCode(&context, req, &rsp);

    if (status.ok())
    {
        rpc_pool_->returnStub(std::move(stub));
        return rsp;
    }
    else
    {
        rpc_pool_->returnStub(std::move(stub));
        rsp.set_error(static_cast<int32_t>(ErrorCode::RPCFAILED));
        return rsp;
    }
}

RPCConnectionPool::RPCConnectionPool(std::size_t size, const std::string &host, const std::string &port)
    : size_(size), host_(host), port_(port), flag_stop_(false)
{
    for (size_t i = 0; i < size_; i++)
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host_ + ":" + port_, grpc::InsecureChannelCredentials());
        stubs_.push(VerifyService::NewStub(channel));
    }
}

RPCConnectionPool::~RPCConnectionPool()
{
    std::lock_guard<std::mutex> lock(mutex_);
    flag_stop_ = true;
    cv_.notify_all();
    while (!stubs_.empty())
    {
        stubs_.pop();
    }
}

std::unique_ptr<VerifyService::Stub> RPCConnectionPool::getStub()
{
    std::unique_lock<std::mutex> lock(mutex_);

    cv_.wait(lock, [this]()
             { return !stubs_.empty() || flag_stop_; });

    if (flag_stop_)
    {
        return nullptr;
    }
    else
    {
        std::unique_ptr<VerifyService::Stub> stub = std::move(stubs_.front());
        stubs_.pop();
        return stub;
    }
}

void RPCConnectionPool::returnStub(std::unique_ptr<VerifyService::Stub> stub)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (flag_stop_)
    {
        return;
    }
    stubs_.push(std::move(stub));
    cv_.notify_one();
}
