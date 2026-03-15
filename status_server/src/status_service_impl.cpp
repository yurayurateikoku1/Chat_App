#include "status_service_impl.h"
#include "common.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "config_mgr.h"
std::string generateUniqueString()
{
    // 创建uuid对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将uuid转换为字符串
    std::string str = boost::uuids::to_string(uuid);
    return str;
}

StatusServiceImpl::StatusServiceImpl()
{
    auto &config_mgr = ConfigMgr::getInstance();
    ChatServer chat_server;
    chat_server.host = config_mgr["chat_server1"]["host"];
    chat_server.port = config_mgr["chat_server1"]["port"];
    chat_server.name = config_mgr["chat_server1"]["name"];
    chat_server.connect_count = 0;
    chat_servers_[chat_server.name] = chat_server;

    chat_server.host = config_mgr["chat_server2"]["host"];
    chat_server.port = config_mgr["chat_server2"]["port"];
    chat_server.name = config_mgr["chat_server2"]["name"];
    chat_server.connect_count = 0;
    chat_servers_[chat_server.name] = chat_server;
}

Status StatusServiceImpl::GetChatServer(ServerContext *context, const GetChatServerReq *request, GetChatServerRsp *response)
{
    const auto &server = getChatServer();
    response->set_host(server.host);
    response->set_port(server.port);
    response->set_error(static_cast<int32_t>(ErrorCode::SUCCESS));
    response->set_token(generateUniqueString());
    insertToken(response->token(), request->uid());
    return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext *context, const LoginReq *request, LoginRsp *response)
{
    auto uid = request->uid();
    auto token = request->token();
    std::lock_guard<std::mutex> lock(token_mutex_);
    auto it = tokens_.find(uid);
    if (it == tokens_.end())
    {
        response->set_error(static_cast<int32_t>(ErrorCode::UIDINVALID));
        return Status::OK;
    }
    if (it->second != token)
    {
        response->set_error(static_cast<int32_t>(ErrorCode::TOKENINVALID));
        return Status::OK;
    }
    response->set_error(static_cast<int32_t>(ErrorCode::SUCCESS));
    response->set_uid(uid);
    response->set_token(token);
    return Status::OK;
}

void StatusServiceImpl::insertToken(const std::string &token, int uid)
{
    std::lock_guard<std::mutex> lock(token_mutex_);
    tokens_[uid] = token;
}

ChatServer StatusServiceImpl::getChatServer()
{
    std::lock_guard<std::mutex> lock(server_mutex_);
    auto min_server = chat_servers_.begin()->second;
    for (const auto &server : chat_servers_)
    {
        if (server.second.connect_count < min_server.connect_count)
        {
            min_server = server.second;
        }
    }
    return min_server;
}
