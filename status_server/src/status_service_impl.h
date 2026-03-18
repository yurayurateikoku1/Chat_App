#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

struct ChatServer
{
    std::string host;
    std::string port;
    std::string name;
    int connect_count;
};

class StatusServiceImpl final : public StatusService::Service
{
public:
    StatusServiceImpl();
    Status GetChatServer(ServerContext *context, const GetChatServerReq *request, GetChatServerRsp *response) override;
    Status Login(ServerContext *context, const LoginReq *request, LoginRsp *response) override;

private:
    void insertToken(const std::string &token, int uid);
    ChatServer getChatServer();
    std::unordered_map<std::string, ChatServer> chat_servers_;
    std::mutex server_mutex_;
};