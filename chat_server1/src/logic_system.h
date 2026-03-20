#pragma once
#include "common.h"
#include "singleton.h"
#include <map>
#include <thread>
#include <queue>
class CSession;
struct LogicNode;
typedef std::function<void(std::shared_ptr<CSession>, short msg_id, const std::string &msg_data)> callBack;

/// @brief 逻辑系统
class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();

    void postMessage2Queue(std::shared_ptr<LogicNode> logic_node);

private:
    LogicSystem();
    void dealMessage();
    void registerCallBacks();
    bool getBaseInfo(const std::string &base_key, int uid, std::shared_ptr<UserInfo> &user_info);
    bool getFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>> &list);
    bool getFriendList(int uid, std::vector<std::shared_ptr<UserInfo>> &list);

    void loginHandler(std::shared_ptr<CSession> session, short msg_id, const std::string &msg_data);
    void searchUserHandler(std::shared_ptr<CSession> session, short msg_id, const std::string &msg_data);
    void addFriendApplyHandler(std::shared_ptr<CSession> session, short msg_id, const std::string &msg_data);
    void authFriendApplyHandler(std::shared_ptr<CSession> session, short msg_id, const std::string &msg_data);
    void ChatTextMsgHandler(std::shared_ptr<CSession> session, short msg_id, const std::string &msg_data);

    std::thread logic_thread_;
    std::queue<std::shared_ptr<LogicNode>> logic_node_queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool flag_stop_;
    std::map<short, callBack> callBacks_;
};