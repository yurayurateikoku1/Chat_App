#include "logic_system.h"
#include "csession.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "redis_mgr.h"
#include "mysql_mgr.h"
#include "msg_node.h"
#include "common.h"
#include "status_grpc_client.h"
#include "user_mgr.h"

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
    try {
    nlohmann::json root = nlohmann::json::parse(msg_data);
    nlohmann::json return_root;
    auto uid = root["uid"].get<int>();
    auto token = root["token"].get<std::string>();
    SPDLOG_INFO("uid:{},token:{}", uid, token);

    Defer defer([this, &return_root, session]()
                {
        std::string msg = return_root.dump();
        SPDLOG_INFO("Sending response: {}", msg);
        session->send(msg,static_cast<short>(MSG_IDS::MSG_CHAT_LOGIN_RSP)); });

    // 从redis查看用户token是否正确
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool ret = RedisMgr::getInstance().getValue(token_key, token_value);
    if (!ret)
    {
        return_root["error"] = ErrorCode::UIDINVALID;
        return;
    }

    std::string base_key = USER_BASE_INFO + uid_str;
    auto user_info = std::make_shared<UserInfo>();
    bool ret_base = getBaseInfo(base_key, uid, user_info);

    return_root["uid"] = uid;
    return_root["passwd"] = user_info->passwd;
    return_root["username"] = user_info->name;
    return_root["email"] = user_info->email;
    return_root["nick"] = user_info->nick;
    return_root["desc"] = user_info->desc;
    return_root["sex"] = user_info->sex;
    return_root["icon"] = user_info->icon;
    return_root["error"] = 0;
    // 从数据库获取申请列表
    // 获取好友列表

    auto server_name = ConfigMgr::getInstance()["self_server"]["name"];
    // 增加登录数量
    auto rd_res = RedisMgr::getInstance().hgetValue(LOGIN_COUNT, server_name);
    int count = 0;
    if (!rd_res.empty())
    {
        count = std::stoi(rd_res);
    }
    count++;
    auto count_str = std::to_string(count);
    RedisMgr::getInstance().hsetValue(LOGIN_COUNT, server_name, count_str);

    //
    session->setUid(uid);
    std::string ip_key = USERIPPREFIX + uid_str;
    RedisMgr::getInstance().setValue(ip_key, server_name);
    UserMgr::getInstance().setUserSession(uid, session);
    } catch (const std::exception &e) {
        SPDLOG_ERROR("loginHandler exception: {}", e.what());
    }
}

bool LogicSystem::getBaseInfo(const std::string &base_key, int uid, std::shared_ptr<UserInfo> &user_info)
{
    std::string string_info = "";
    bool ret = RedisMgr::getInstance().getValue(base_key, string_info);
    if (ret)
    {
        nlohmann::json root = nlohmann::json::parse(string_info);
        user_info->uid = root["uid"].get<int>();
        user_info->name = root["name"].get<std::string>();
        user_info->passwd = root["passwd"].get<std::string>();
        user_info->email = root["email"].get<std::string>();
        user_info->nick = root["nick"].get<std::string>();
        user_info->desc = root["desc"].get<std::string>();
        user_info->sex = root["sex"].get<int>();
        user_info->icon = root["icon"].get<std::string>();
    }
    else
    {
        std::shared_ptr<UserInfo> mysql_info = nullptr;
        mysql_info = MysqlMgr::getInstance().getUser(uid);
        if (mysql_info == nullptr)
        {
            return false;
        }

        user_info = mysql_info;

        nlohmann::json redis_root;
        redis_root["uid"] = user_info->uid;
        redis_root["name"] = user_info->name;
        redis_root["passwd"] = user_info->passwd;
        redis_root["email"] = user_info->email;
        redis_root["nick"] = user_info->nick;
        redis_root["desc"] = user_info->desc;
        redis_root["sex"] = user_info->sex;
        redis_root["icon"] = user_info->icon;
        RedisMgr::getInstance().setValue(base_key, redis_root.dump());
    }
    return true;
}
