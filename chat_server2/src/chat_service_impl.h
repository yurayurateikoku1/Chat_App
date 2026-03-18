#pragma once
#include "common.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <queue>
#include <mutex>

using grpc::Channel;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::ChatService;
using message::TextChatData;
using message::TextChatMsgReq;
using message::TextChatMsgRsp;

class ChatServiceImpl final : public ChatService::Service
{
public:
    ChatServiceImpl();
    ~ChatServiceImpl();

    Status NotifyAddFriend(ServerContext *context, const AddFriendReq *request, AddFriendRsp *response) override;
    Status NotifyAuthFriend(ServerContext *context, const AuthFriendReq *request, AuthFriendRsp *response) override;
    Status NotifyTextChatMsg(ServerContext *context, const TextChatMsgReq *request, TextChatMsgRsp *response) override;
    bool getBaseInfo(const std::string &base_key, int uid, std::shared_ptr<UserInfo> &user_info);

private:
};