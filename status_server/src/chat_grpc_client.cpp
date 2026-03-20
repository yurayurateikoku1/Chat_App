#include "chat_grpc_client.h"
#include "config_mgr.h"
ChatConnectionPool::ChatConnectionPool(size_t pool_size, const std::string &host, const std::string &port)
    : pool_size_(pool_size), host_(host), port_(port), flag_stop_(false)
{
    for (size_t i = 0; i < pool_size_; i++)
    {
        std::unique_ptr<ChatService::Stub> stub = ChatService::NewStub(grpc::CreateChannel(host_ + ":" + port_, grpc::InsecureChannelCredentials()));
        stubs_.push(std::move(stub));
    }
}

ChatConnectionPool::~ChatConnectionPool()
{
    std::lock_guard<std::mutex> lock(mutex_);
    flag_stop_ = true;
    cv_.notify_all();
    while (!stubs_.empty())
    {
        stubs_.pop();
    }
}

std::unique_ptr<ChatService::Stub> ChatConnectionPool::getConnection()
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

void ChatConnectionPool::returnConnection(std::unique_ptr<ChatService::Stub> stub)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (flag_stop_)
    {
        return;
    }
    stubs_.push(std::move(stub));
    cv_.notify_one();
}

ChatGrpcClient::ChatGrpcClient()
{
    auto &config_mgr = ConfigMgr::getInstance();
    auto server_list = config_mgr["chat_servers"]["name"];

    std::vector<std::string> words;
    std::stringstream ss(server_list);
    std::string word;

    while (std::getline(ss, word, ','))
    {
        words.push_back(word);
    }

    for (auto &word : words)
    {
        if (config_mgr[word]["name"].empty())
        {
            continue;
        }
        pool_map_[config_mgr[word]["name"]] = std::make_unique<ChatConnectionPool>(5, config_mgr[word]["host"], config_mgr[word]["port"]);
    }
}

ChatGrpcClient::~ChatGrpcClient()
{
}

AuthFriendRsp ChatGrpcClient::notifyAuthFriend(const std::string &server_ip, const AuthFriendReq &req)
{
    AuthFriendRsp rsp;
    return rsp;
}

TextChatMsgRsp ChatGrpcClient::notifyTextChatMsg(const std::string &server_ip, const TextChatMsgReq &req, const nlohmann::json &json)
{
    TextChatMsgRsp rsp;
    return rsp;
}

AddFriendRsp ChatGrpcClient::notifyAddFriend(const std::string &server_ip, const AddFriendReq &req)
{
    auto to_uid = req.touid();
    std::string uid_str = std::to_string(to_uid);
    AddFriendRsp rsp;
    return rsp;
}
