#include "logic_system.h"
#include "csession.h"
LogicSystem::LogicSystem()
{
    handleRegisterGetRequest("/get_test", [](std::shared_ptr<CSession> session)
                             { beast::ostream(session->response_.body()) << "receive get_test req\r\n"; });
}

LogicSystem::~LogicSystem()
{
}

bool LogicSystem::handleGetRequest(const std::string &url, std::shared_ptr<CSession> session)
{
    if (get_handlers_.find(url) == get_handlers_.end())
    {
        return false;
    }

    get_handlers_[url](session);
    return true;
}

void LogicSystem::handleRegisterGetRequest(const std::string &url, httpHandler handler)
{
    get_handlers_.insert({url, handler});
}
