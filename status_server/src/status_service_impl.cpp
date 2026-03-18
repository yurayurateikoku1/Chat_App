#include "status_service_impl.h"
#include "common.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "config_mgr.h"
#include "redis_mgr.h"
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
        ChatServer server;
        server.host = config_mgr[word]["host"];
        server.port = config_mgr[word]["port"];
        server.name = config_mgr[word]["name"];
        chat_servers_[server.name] = server;
    }
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

    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool ret = RedisMgr::getInstance().getValue(token_key, token_value);
    if (!ret)
    {
        response->set_error(static_cast<int32_t>(ErrorCode::UIDINVALID));
        return Status::OK;
    }

    if (token_value != token)
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
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    RedisMgr::getInstance().setValue(token_key, token);
}

ChatServer StatusServiceImpl::getChatServer()
{
    std::lock_guard<std::mutex> lock(server_mutex_);
    auto min_server = chat_servers_.begin()->second;

    auto count_str = RedisMgr::getInstance().hgetValue(LOGIN_COUNT, min_server.name);

    if (count_str.empty())
    {
        min_server.connect_count = INT_MAX;
    }
    else
    {
        min_server.connect_count = std::stoi(count_str);
    }

    for (auto &server : chat_servers_)
    {
        if (server.second.name == min_server.name)
        {
            continue;
        }

        auto count_str = RedisMgr::getInstance().hgetValue(LOGIN_COUNT, server.second.name);
        if (count_str.empty())
        {
            server.second.connect_count = INT_MAX;
        }
        else
        {
            server.second.connect_count = std::stoi(count_str);
        }

        if (server.second.connect_count < min_server.connect_count)
        {
            min_server = server.second;
        }
    }

    return min_server;
}
