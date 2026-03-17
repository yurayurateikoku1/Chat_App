#include "logic_system.h"
#include "csession.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "redis_mgr.h"
#include "mysql_mgr.h"
#include "msg_node.h"
#include "status_grpc_client.h"

LogicSystem::LogicSystem()
    : flag_stop_(false)
{
    registerCallBacks();
    logic_thread_ = std::thread(&LogicSystem::dealMessage, this);
}

LogicSystem::~LogicSystem()
{
    flag_stop_ = true;
    cv_.notify_all();
    logic_thread_.join();
}

void LogicSystem::postMessage2Queue(std::shared_ptr<LogicNode> logic_node)
{
    std::unique_lock<std::mutex> lock(mutex_);
    logic_node_queue_.push(logic_node);
    if (logic_node_queue_.size() == 1)
    {
        lock.unlock();
        cv_.notify_one();
    }
}

void LogicSystem::dealMessage()
{
    for (;;)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]
                 { return !logic_node_queue_.empty() || flag_stop_; });
        // 等待所有逻辑执行完才退出
        if (flag_stop_)
        {
            while (!logic_node_queue_.empty())
            {
                auto logic_node = logic_node_queue_.front();
                auto it = callBacks_.find(logic_node->recv_node_->message_id_);
                if (it == callBacks_.end())
                {
                    logic_node_queue_.pop();
                    continue;
                }
                it->second(logic_node->session_, logic_node->recv_node_->message_id_, std::string(logic_node->recv_node_->buf_, logic_node->recv_node_->current_len_));
                logic_node_queue_.pop();
            }
            break;
        }

        auto logic_node = logic_node_queue_.front();
        auto it = callBacks_.find(logic_node->recv_node_->message_id_);
        if (it == callBacks_.end())
        {
            logic_node_queue_.pop();
            continue;
        }
        it->second(logic_node->session_, logic_node->recv_node_->message_id_, std::string(logic_node->recv_node_->buf_, logic_node->recv_node_->current_len_));
        logic_node_queue_.pop();
    }
}

void LogicSystem::registerCallBacks()
{
    callBacks_.insert({static_cast<short>(MSG_IDS::MSG_CHAT_LOGIN), std::bind(&LogicSystem::loginHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
}

void LogicSystem::loginHandler(std::shared_ptr<CSession> session, short msg_id, const std::string &msg_data)
{
    nlohmann::json root = nlohmann::json::parse(msg_data);
    auto uid = root["uid"].get<int>();
    SPDLOG_INFO("uid:{}", uid);
    // 从状态服务器获取token匹配是否正确
    SPDLOG_INFO("Calling StatusGrpcClient::login for uid:{}", uid);
    auto respone = StatusGrpcClient::getInstance().login(uid, root["token"].get<std::string>());
    SPDLOG_INFO("StatusGrpcClient::login returned, error:{}", respone.error());
    nlohmann::json return_root;
    Defer defer([this, &return_root, session]()
                {
        std::string msg = return_root.dump();
        SPDLOG_INFO("Sending response: {}", msg);
        session->send(msg,static_cast<short>(MSG_IDS::MSG_CHAT_LOGIN_RSP)); });

    return_root["root"] = respone.error();
    if (respone.error() != 0)
    {
        SPDLOG_ERROR("Login error: {}", respone.error());
        return;
    }

    // 从数据查看是否存在该用户
    std::shared_ptr<UserInfo> user_info = nullptr;
    SPDLOG_INFO("Querying MySQL for uid:{}", uid);
    user_info = MysqlMgr::getInstance().getUser(uid);
    if (user_info == nullptr)
    {
        SPDLOG_ERROR("User not found for uid:{}", uid);
        return_root["error"] = static_cast<int>(ErrorCode::UIDINVALID);
        return;
    }
    SPDLOG_INFO("User found: name={}", user_info->name);
    return_root["error"] = 0;
    return_root["uid"] = uid;
    return_root["token"] = respone.token();
    return_root["name"] = user_info->name;
}
