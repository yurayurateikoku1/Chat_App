#include "chat_service_impl.h"
#include "user_mgr.h"
#include <nlohmann/json.hpp>
#include "csession.h"
#include "redis_mgr.h"
#include "mysql_mgr.h"
#include <spdlog/spdlog.h>

ChatServiceImpl::ChatServiceImpl()
{
}

ChatServiceImpl::~ChatServiceImpl()
{
}

Status ChatServiceImpl::NotifyAddFriend(ServerContext *context, const AddFriendReq *request, AddFriendRsp *response)
{
    auto to_uid = request->touid();
    auto session = UserMgr::getInstance().getCSeSsion(to_uid);

    Defer defer([request, response]()
                { 
                    response->set_error(static_cast<int32_t>(ErrorCode::SUCCESS));
                    response->set_applyuid(request->applyuid());
                    response->set_touid(request->touid()); });

    if (session == nullptr)
    {
        return Status::OK;
    }

    nlohmann::json root;
    root["error"] = 0;
    root["apply_uid"] = request->applyuid();
    root["username"] = request->name();
    root["desc"] = request->desc();
    root["icon"] = request->icon();
    root["sex"] = request->sex();
    root["nick"] = request->nick();

    session->send(root.dump(), static_cast<short>(MSG_IDS::ID_NOTIFY_ADD_FRIEND_REQ));
    return Status::OK;
}

Status ChatServiceImpl::NotifyAuthFriend(ServerContext *context, const AuthFriendReq *request, AuthFriendRsp *response)
{
    auto to_uid = request->touid();
    auto from_id = request->fromuid();

    auto session = UserMgr::getInstance().getCSeSsion(to_uid);

    Defer defer([request, response]()
                {
        response->set_error(static_cast<int32_t>(ErrorCode::SUCCESS));
        response->set_fromuid(request->fromuid());
        response->set_touid(request->touid()); });

    if (session == nullptr)
    {
        return Status::OK;
    }

    nlohmann::json root;
    root["error"] = 0;
    root["from_uid"] = request->fromuid();
    root["to_uid"] = request->touid();

    std::string base_key = USER_BASE_INFO + std::to_string(from_id);
    auto user_info = std::make_shared<UserInfo>();
    bool ret = getBaseInfo(base_key, from_id, user_info);
    if (ret)
    {
        root["username"] = user_info->name;
        root["nick"] = user_info->nick;
        root["icon"] = user_info->icon;
        root["sex"] = user_info->sex;
    }
    else
    {
        root["error"] = static_cast<int32_t>(ErrorCode::UIDINVALID);
    }
    session->send(root.dump(), static_cast<short>(MSG_IDS::ID_NOTIFY_AUTH_FRIEND_REQ));

    return Status::OK;
}

Status ChatServiceImpl::NotifyTextChatMsg(ServerContext *context, const TextChatMsgReq *request, TextChatMsgRsp *response)
{
    auto to_uid = request->touid();
    auto session = UserMgr::getInstance().getCSeSsion(to_uid);
    response->set_error(static_cast<int32_t>(ErrorCode::SUCCESS));

    if (session == nullptr)
    {
        return Status::OK;
    }

    nlohmann::json root;
    nlohmann::json text_array = nlohmann::json::array();
    root["error"] = 0;
    root["from_uid"] = request->fromuid();
    root["to_uid"] = request->touid();

    for (const auto &item : request->textmsgs())
    {
        nlohmann::json msg_obj;
        msg_obj["msgid"] = item.msgid();
        msg_obj["content"] = item.msgcontent();
        text_array.push_back(msg_obj);
    }
    root["text_array"] = text_array;
    session->send(root.dump(), static_cast<short>(MSG_IDS::ID_NOTIFY_TEXT_CHAT_MSG_REQ));

    return Status::OK;
}

bool ChatServiceImpl::getBaseInfo(const std::string &base_key, int uid, std::shared_ptr<UserInfo> &user_info)
{
    std::string string_info = "";
    bool ret = RedisMgr::getInstance().getValue(base_key, string_info);
    if (ret)
    {
        nlohmann::json root = nlohmann::json::parse(string_info);
        user_info->uid = root["uid"].get<int>();
        user_info->name = root["username"].get<std::string>();
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
        redis_root["username"] = user_info->name;
        redis_root["nick"] = user_info->nick;
        redis_root["desc"] = user_info->desc;
        redis_root["sex"] = user_info->sex;
        redis_root["icon"] = user_info->icon;
        RedisMgr::getInstance().setValue(base_key, redis_root.dump());
    }
    return true;
}
