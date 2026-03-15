#include "login_page.h"
#include <QRegularExpression>
#include "../http_mgr.h"
#include "../tcp_mgr.h"
#include <spdlog/spdlog.h>

LoginPage::LoginPage(QObject *parent) : QObject(parent)
{
    initHttpHandler();
    connect(HttpMgr::getInstance().get(), &HttpMgr::signLoginModuleDone, this, &LoginPage::handleLoginModule);
    // tcp连接状态
    connect(TCPMgr::getInstance().get(), &TCPMgr::signTcpConnectionStatus, this, [this](bool status)
            {
            if (status)
            {
                emit sign2UIMessage("Logining...", true);
                QJsonObject json;
                json["uid"]=uid_;
                json["token"]=QString::fromStdString(token_);

                QJsonDocument doc(json);
                QString json_str = doc.toJson(QJsonDocument::Compact);
                
                //通过TCP向ChatServer发送登录请求
                QMetaObject::invokeMethod(TCPMgr::getInstance().get(), [json_str]
                                          { TCPMgr::getInstance()->tcpSendData(ReqId::ID_CHAT_LOGIN,json_str); }, Qt::QueuedConnection);
            }
            else
            {
                emit sign2UIMessage("Network request failed!", false);
            } });
    // chatServer登录状态
    connect(TCPMgr::getInstance().get(), &TCPMgr::signLoginChatStatus, this, [this](bool status)
            { 
                emit sign2UILoginStatus(status); 

                SPDLOG_INFO("Login status: {}", status); });
}

void LoginPage::loginUser(const QString &email, const QString &password)
{
    if (email == "" || password == "")
    {
        emit sign2UIMessage("The input item cannot be empty!", false);
        return;
    }

    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool math = regex.match(email).hasMatch();
    if (!math)
    {
        emit sign2UIMessage("The email format is incorrect!", false);
        return;
    }

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;
    HttpMgr::getInstance()->sendPostRequest(QUrl(gate_url_prefix + "/user_login"), json, ReqId::ID_LOGIN_USER, Modules::LOGIN);
}

void LoginPage::initHttpHandler()
{
    // 注册用户登录回包后处理
    http_handlers_.insert({ReqId::ID_LOGIN_USER, [this](const QJsonObject &obj)
                           {
                               int erro_code = obj.value("error").toInt();
                               if (!erro_code == 0)
                               {
                                   emit sign2UIMessage("Parameter error!", false);
                                   emit sign2UILoginStatus(false);
                                   return;
                               }
                               auto email = obj.value("email").toString();

                               ServerInfo server_info;
                               server_info.host = obj.value("host").toString().toStdString();
                               server_info.port = obj.value("port").toString().toStdString();
                               server_info.token = obj.value("token").toString().toStdString();
                               server_info.uid = obj.value("uid").toInt();

                               uid_ = server_info.uid;
                               token_ = server_info.token;
                               SPDLOG_INFO("tcp connect to host: {},port: {}", server_info.host, server_info.port);
                               // 长连接tcp
                               QMetaObject::invokeMethod(TCPMgr::getInstance().get(), [server_info]
                                                         { TCPMgr::getInstance()->tcpConnection(server_info); }, Qt::QueuedConnection);
                           }});
}

void LoginPage::handleLoginModule(ReqId id, const std::string &res, ErrorCode code)
{
    if (code != ErrorCode::SUCCESS)
    {
        emit sign2UIMessage("Network request failed!", false);
        return;
    }

    // 解析json
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(res));
    if (doc.isNull())
    {
        emit sign2UIMessage("Json parse failed!", false);
        return;
    }
    if (!doc.isObject())
    {
        emit sign2UIMessage("Json parse failed!", false);
        return;
    }

    // 处理对应功能的后处理
    http_handlers_[id](doc.object());
}
