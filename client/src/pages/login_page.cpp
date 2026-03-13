#include "login_page.h"
#include <QRegularExpression>
#include "../http_mgr.h"
#include <spdlog/spdlog.h>

LoginPage::LoginPage(QObject *parent) : QObject(parent)
{
    initHttpHandler();
    connect(HttpMgr::getInstance().get(), &HttpMgr::signLoginModuleDone, this, &LoginPage::slotLoginModuleDone);
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
                               SPDLOG_INFO("{} ,uid {} login succeeded!", email.toStdString(), server_info.uid);
                               emit sign2UILoginStatus(true);
                           }});
}

void LoginPage::slotLoginModuleDone(ReqId id, const std::string &res, ErrorCode code)
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
