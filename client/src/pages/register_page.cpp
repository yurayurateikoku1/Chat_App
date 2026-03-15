#include "register_page.h"
#include <QRegularExpression>
#include "../http_mgr.h"
#include <spdlog/spdlog.h>
RegisterPage::RegisterPage(QObject *parent)
{
    connect(HttpMgr::getInstance().get(), &HttpMgr::signRegisterModuleDone, this, &RegisterPage::handleRegisterModule);
    initHttpHandler();
}

Q_INVOKABLE void RegisterPage::getVerifyCode(const QString &email)
{
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool math = regex.match(email).hasMatch();
    if (math)
    {
        QJsonObject json;
        json["email"] = email;
        HttpMgr::getInstance()->sendPostRequest(QUrl(gate_url_prefix + "/get_verifycode"), json, ReqId::ID_GET_VERIFY_CODE, Modules::REGISTER);
    }
    else
    {
        emit sign2UIMessage("The email format is incorrect!", false);
    }
}

Q_INVOKABLE void RegisterPage::registerUser(const QString &username, const QString &password, const QString &confirm_password, const QString &email, const QString &verify_code)
{
    if (username == "" || password == "" || email == "" || verify_code == "")
    {
        emit sign2UIMessage("The input item cannot be empty!", false);
        return;
    }

    if (password != confirm_password)
    {
        emit sign2UIMessage("The two passwords are inconsistent!", false);
        return;
    }

    QJsonObject json;
    json["username"] = username;
    json["password"] = password;
    json["email"] = email;
    json["verifycode"] = verify_code;
    HttpMgr::getInstance()->sendPostRequest(QUrl(gate_url_prefix + "/user_register"), json, ReqId::ID_REG_USER, Modules::REGISTER);
}

void RegisterPage::initHttpHandler()
{
    // 获取验证码后处理
    http_handlers_.insert({ReqId::ID_GET_VERIFY_CODE, [this](const QJsonObject &obj)
                           {
                               int erro_code = obj.value("error").toInt();
                               if (!erro_code == 0)
                               {
                                   emit sign2UIMessage("Parameter error!", false);
                                   return;
                               }

                               auto email = obj.value("email").toString();
                               emit sign2UIMessage("The verification code has been sent to " + email, true);
                               SPDLOG_INFO("The verification code has been sent to {}", email.toStdString());
                           }});

    // 注册用户后处理
    http_handlers_.insert({ReqId::ID_REG_USER, [this](const QJsonObject &obj)
                           {
                               int erro_code = obj.value("error").toInt();
                               if (!erro_code == 0)
                               {
                                   emit sign2UIMessage("Parameter error!", false);
                                   return;
                               }
                               auto email = obj.value("email").toString();
                               auto uid = obj.value("uid").toInt();
                               emit sign2UIRegisterSuccess(email);
                               SPDLOG_INFO("{} ,uid {} register succeeded!", email.toStdString(), uid);
                           }});
}

void RegisterPage::handleRegisterModule(ReqId id, const std::string &res, ErrorCode code)
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
