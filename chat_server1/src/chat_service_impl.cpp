#include "chat_service_impl.h"
#include "user_mgr.h"
#include <nlohmann/json.hpp>
#include "csession.h"
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
    return Status::OK;
}

Status ChatServiceImpl::NotifyTextChatMsg(ServerContext *context, const TextChatMsgReq *request, TextChatMsgRsp *response)
{
    return Status::OK;
}

bool ChatServiceImpl::getBaseInfo(const std::string &base_key, int uid, std::shared_ptr<UserInfo> &user_info)
{
    return true;
}
