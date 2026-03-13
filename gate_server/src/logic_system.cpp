#include "logic_system.h"
#include "csession.h"
#include "verify_grpc_client.h"
#include "status_grpc_client.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "redis_mgr.h"
#include "mysql_mgr.h"
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
    // 注册post获取验证码请求处理
    registerHandlePostReqCallback("/get_verifycode", [](std::shared_ptr<CSession> session)
                                  {
                                      auto body_str = boost::beast::buffers_to_string(session->request_.body().data());
                                      SPDLOG_INFO("body_str:{}", body_str);
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
                                          root["error"] = static_cast<int>(ErrorCode::ERROR_JSON);
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      }

                                      if (!src_root.contains("email"))
                                      {
                                          SPDLOG_ERROR("Missing email field");
                                          root["error"] = static_cast<int>(ErrorCode::ERROR_JSON);
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      }

                                      auto email = src_root["email"].get<std::string>();
                                      SPDLOG_DEBUG("email:{}", email);
                                      // 发送grpc请求获取验证码
                                      GetVerifyRsp rsp = VerifyGrpcClient::getInstance()->getVerifyCode(email);
                                      

                                      root["error"] = rsp.error();
                                      root["email"] = src_root["email"];
                                      beast::ostream(session->response_.body()) << root.dump(); });

    // 注册post注册用户请求处理
    registerHandlePostReqCallback("/user_register", [](std::shared_ptr<CSession> session)
                                  {
                                      auto body_str = boost::beast::buffers_to_string(session->request_.body().data());
                                      SPDLOG_INFO("body_str:{}", body_str);
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
                                          root["error"] = static_cast<int>(ErrorCode::ERROR_JSON);
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      }

                                      std::string verify_code;
                                      // 在redis中查找验证码
                                      bool return_verify = RedisMgr::getInstance().getValue(CODEPREFIX + src_root["email"].get<std::string>(), verify_code);
                                      if (!return_verify)
                                      {
                                          SPDLOG_WARN("Failed to get verify_code from redis");
                                          root["error"] = static_cast<int>(ErrorCode::VERIFYEXPIRED);
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      }

                                      if (verify_code != src_root["verifycode"].get<std::string>())
                                      {
                                          SPDLOG_WARN("verify_code is not match");
                                          root["error"] = static_cast<int>(ErrorCode::ERROR_VERIFYCODE);
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      }

                                      //查找数据库判断用户是否存在
                                      int uid = MysqlMgr::getInstance().registerUser(src_root["username"].get<std::string>(), src_root["password"].get<std::string>(), src_root["email"].get<std::string>());
                                      if(uid==0||uid==-1)
                                      {
                                        SPDLOG_ERROR("User or email is exist");
                                        root["error"] = static_cast<int>(ErrorCode::USEREXISTED);
                                        beast::ostream(session->response_.body()) << root.dump();
                                        return;
                                      }


                                      root["error"] = static_cast<int>(ErrorCode::SUCCESS);
                                      root["email"] = src_root["email"];
                                      root["uid"] = uid;
                                      root["password"] = src_root["password"];
                                      root["username"] = src_root["username"];
                                      root["verifycode"] = src_root["verifycode"];
                                      beast::ostream(session->response_.body()) << root.dump(); });

    // 注册post重置密码请求处理
    registerHandlePostReqCallback("/reset_password", [](std::shared_ptr<CSession> session)
                                  {
        auto body_str = boost::beast::buffers_to_string(session->request_.body().data());
        SPDLOG_INFO("body_str:{}", body_str);
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
            root["error"] = static_cast<int>(ErrorCode::ERROR_JSON);
            beast::ostream(session->response_.body()) << root.dump();
            return;
        }

        auto email = src_root["email"].get<std::string>();
        auto password = src_root["password"].get<std::string>();
        auto username = src_root["username"].get<std::string>();

        std::string verify_code;
        // 在redis中查找验证码
        bool return_verify = RedisMgr::getInstance().getValue(CODEPREFIX + email, verify_code);
        if (!return_verify)
        {
            SPDLOG_WARN("Failed to get verify_code from redis");
            root["error"] = static_cast<int>(ErrorCode::VERIFYEXPIRED);
            beast::ostream(session->response_.body()) << root.dump();
            return;
        }

        if (verify_code != src_root["verifycode"].get<std::string>())
        {
            SPDLOG_WARN("verify_code is not match");
            root["error"] = static_cast<int>(ErrorCode::ERROR_VERIFYCODE);
            beast::ostream(session->response_.body()) << root.dump();
            return;
        }

        // 查找数据库判断用户和邮箱是否匹配
        bool return_user = MysqlMgr::getInstance().checkEmail(username, email);
        if (!return_user)
        {
            SPDLOG_WARN("User or email is not match");
            root["error"] = static_cast<int>(ErrorCode::EMAILNOTMATCH);
            beast::ostream(session->response_.body()) << root.dump();
            return;
        } 
        
        //更新密码
        bool return_update = MysqlMgr::getInstance().updatePassword(username, password);
        if (!return_update)
        {
            SPDLOG_ERROR("Failed to update password");
            root["error"] = static_cast<int>(ErrorCode::PASSWDUPFAILED);
            beast::ostream(session->response_.body()) << root.dump();
            return;
        }
        
        root["error"] = static_cast<int>(ErrorCode::SUCCESS);
        root["email"] = src_root["email"];
        root["username"] = src_root["username"];
        beast::ostream(session->response_.body()) << root.dump(); });

    // 注册post登录请求处理
    registerHandlePostReqCallback("/user_login", [](std::shared_ptr<CSession> session)
                                  {
                                      auto body_str = boost::beast::buffers_to_string(session->request_.body().data());
                                      SPDLOG_INFO("body_str:{}", body_str);
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
                                          root["error"] = static_cast<int>(ErrorCode::ERROR_JSON);
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      }

                                      auto email = src_root["email"].get<std::string>();
                                      auto password = src_root["password"].get<std::string>();
                                      UserInfo user_info;

                                      // 查找数据库判断邮箱和密码是否正确
                                      bool return_user = MysqlMgr::getInstance().checkPassword(email, password, user_info);
                                      if (!return_user)
                                      {
                                          SPDLOG_WARN("User or password is not match");
                                          root["error"] = static_cast<int>(ErrorCode::PASSWDUPFAILED);
                                          beast::ostream(session->response_.body()) << root.dump();
                                          return;
                                      } 
                                          // 查询StatusServer找到合适的连接
                                        auto reply = StatusGrpcClient::getInstance().getChatServer(user_info.uid);
                                        if (reply.error())
                                        {
                                            SPDLOG_ERROR("Failed to get chat server, error code: {}", reply.error());
                                            root["error"] = static_cast<int>(ErrorCode::RPCFAILED);
                                            beast::ostream(session->response_.body()) << root.dump();
                                            return;
                                        } 
                                        
                                        SPDLOG_INFO("succeed to load userinfo:{}", user_info.uid);
                                        root["error"] = static_cast<int>(ErrorCode::SUCCESS);
                                        root["email"] = src_root["email"];
                                        root["username"] = src_root["username"];
                                        root["uid"] = user_info.uid;
                                        root["token"] = reply.token();
                                        root["host"] = reply.host();
                                        root["port"] = reply.port();
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
