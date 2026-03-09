#pragma once
#include "common.h"
#include "singleton.h"
#include <map>

class CSession;
typedef std::function<void(std::shared_ptr<CSession>)> httpHandler;

/// @brief 逻辑系统
class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();

    bool handleGetRequest(const std::string &url, std::shared_ptr<CSession> session);

    bool handlePostRequest(const std::string &url, std::shared_ptr<CSession> session);

    /// @brief 注册get请求回调
    /// @param url
    /// @param handler
    void registerHandleGetReqCallback(const std::string &url, httpHandler handler);

    /// @brief 注册post请求回调
    /// @param url
    /// @param handler
    void registerHandlePostReqCallback(const std::string &url, httpHandler handler);

private:
    LogicSystem();
    std::map<std::string, httpHandler> post_handlers_;
    std::map<std::string, httpHandler> get_handlers_;
};