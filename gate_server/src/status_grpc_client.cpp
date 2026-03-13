#include "status_grpc_client.h"

StatusConnectionPool::StatusConnectionPool(size_t pool_size, const std::string &host, const std::string &port)
    : pool_size_(pool_size), host_(host), port_(port)
{
    for (size_t i = 0; i < pool_size_; i++)
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(host_ + ":" + port_, grpc::InsecureChannelCredentials());
        stubs_.push(StatusService::NewStub(channel));
    }
}

StatusConnectionPool::~StatusConnectionPool()
{
    std::lock_guard<std::mutex> lock(mutex_);
    flag_stop_ = true;
    cv_.notify_all();
    while (!stubs_.empty())
    {
        stubs_.pop();
    }
}

std::unique_ptr<StatusService::Stub> StatusConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]
             { return !stubs_.empty() || flag_stop_; });
    if (flag_stop_)
    {
        return nullptr;
    }
    auto connection = std::move(stubs_.front());
    stubs_.pop();
    return connection;
}

void StatusConnectionPool::returnConnection(std::unique_ptr<StatusService::Stub> stub)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (flag_stop_)
    {
        return;
    }
    stubs_.push(std::move(stub));
    cv_.notify_one();
}

GetChatServerRsp StatusGrpcClient::getChatServer(int uid)
{
    ClientContext context;
    GetChatServerReq req;
    req.set_uid(uid);
    GetChatServerRsp rsp;
    auto stub = status_pool_->getConnection();
    Status status = stub->GetChatServer(&context, req, &rsp);
    Defer defer([this, &stub]
                { status_pool_->returnConnection(std::move(stub)); });

    if (!status.ok())
    {
        rsp.set_error(static_cast<int32_t>(ErrorCode::RPCFAILED));
        return rsp;
    }
    return rsp;
}

LoginRsp StatusGrpcClient::login(int uid, const std::string &token)
{
    return LoginRsp();
}

StatusGrpcClient::StatusGrpcClient()
{
    auto &config_mgr = ConfigMgr::getInstance();
    auto host = config_mgr["status_server"]["host"];
    auto port = config_mgr["status_server"]["port"];
    status_pool_.reset(new StatusConnectionPool(5, host, port));
}
