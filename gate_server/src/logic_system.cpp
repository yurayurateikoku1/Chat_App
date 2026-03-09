#include "logic_system.h"
#include "csession.h"
#include "verify_grpc_client.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
LogicSystem::LogicSystem()
{
    // 注册get请求处理
    registerHandleGetReqCallback("/get_test", [](std::shared_ptr<CSession> session)
                                 {
                                   beast::ostream(session->response_.body()) << "receive get_test req\r\n";
                                   int i = 0;
                                   for (auto &elem : session->get_params_)
                                   {
                                       i++;
                                       beast::ostream(session->response_.body()) << elem.first << ":" << elem.second << "\r\n";
                                   } });
    // 注册post请求处理
    registerHandlePostReqCallback("/get_verifycode", [](std::shared_ptr<CSession> session)
                                  {
                                      auto body_str = boost::beast::buffers_to_string(session->request_.body().data());
                                      SPDLOG_DEBUG("body_str:{}", body_str);
                                      session->response_.set(http::field::content_type, "text/json");

                                      nlohmann::json root;
                                      nlohmann::json src_root;

                                      try
                                      {
                                          src_root = nlohmann::json::parse(body_str);
                                      }
                                      catch (const nlohmann::json::exception &e)
                                      {
                                          SPDLOG_ERROR("Failed to parse JSON: {}", e.what());
                                          root["error"] = ErrorCode::ERROR_JSON;
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      }

                                      if (!src_root.contains("email"))
                                      {
                                          SPDLOG_ERROR("Missing email field");
                                          root["error"] = ErrorCode::ERROR_JSON;
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      }

                                      auto email = src_root["email"].get<std::string>();
                                      // 发送grpc请求获取验证码
                                      GetVerifyRsp rsp = VerifyGrpcClient::getInstance()->getVerifyCode(email);
                                      SPDLOG_DEBUG("email:{}", email);

                                      root["error"] = rsp.error();
                                      root["email"] = src_root["email"];
                                      beast::ostream(session->response_.body()) << root.dump(); });
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

bool LogicSystem::handlePostRequest(const std::string &url, std::shared_ptr<CSession> session)
{
    if (post_handlers_.find(url) == post_handlers_.end())
    {
        return false;
    }
    post_handlers_[url](session);
    return true;
}

void LogicSystem::registerHandleGetReqCallback(const std::string &url, httpHandler handler)
{
    get_handlers_.insert({url, handler});
}

void LogicSystem::registerHandlePostReqCallback(const std::string &url, httpHandler handler)
{
    post_handlers_.insert({url, handler});
}
