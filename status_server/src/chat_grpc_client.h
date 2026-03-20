#pragma once
#include "common.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <nlohmann/json.hpp>
#include <queue>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::ChatService;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;

using message::TextChatData;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;

using message::KickUserReq;
using message::KickUserRsp;

class ChatConnectionPool
{
public:
    ChatConnectionPool(size_t pool_size, const std::string &host, const std::string &port);
    ~ChatConnectionPool();
    std::unique_ptr<ChatService::Stub> getConnection();
    void returnConnection(std::unique_ptr<ChatService::Stub> stub);

private:
    std::size_t pool_size_;
    std::string host_;
    std::string port_;
    std::atomic<bool> flag_stop_;
    std::queue<std::unique_ptr<ChatService::Stub>> stubs_;
    std::condition_variable cv_;
    std::mutex mutex_;
};

class ChatGrpcClient
{

public:
    static ChatGrpcClient &getInstance()
    {
        static ChatGrpcClient instance;
        return instance;
    }
    ChatGrpcClient(const ChatGrpcClient &) = delete;
    ChatGrpcClient &operator=(const ChatGrpcClient &) = delete;
    ~ChatGrpcClient();

    /// @brief
    /// @param server_ip
    /// @param req
    /// @return
    AddFriendRsp notifyAddFriend(const std::string &server_ip, const AddFriendReq &req);

    /// @brief
    /// @param server_ip
    /// @param req
    /// @return
    AuthFriendRsp notifyAuthFriend(const std::string &server_ip, const AuthFriendReq &req);

    TextChatMsgRsp notifyTextChatMsg(const std::string &server_ip, const TextChatMsgReq &req,
                                     const nlohmann::json &json);

private:
    ChatGrpcClient();
    std::unordered_map<std::string, std::unique_ptr<ChatConnectionPool>> pool_map_;
};