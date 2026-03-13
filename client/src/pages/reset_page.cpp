#include "reset_page.h"
#include <QRegularExpression>
#include "../http_mgr.h"
#include <spdlog/spdlog.h>
ResetPage::ResetPage(QObject *parent)
{
    initHttpHandler();
    connect(HttpMgr::getInstance().get(), &HttpMgr::signResetModuleDone, this, &ResetPage::slotResetModuleDone);
}

Q_INVOKABLE void ResetPage::getVerifyCode(const QString &email)
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

Q_INVOKABLE void ResetPage::resetPassword(const QString &username, const QString &new_password, const QString &email, const QString &verify_code)
{
    if (username == "" || new_password == "" || email == "" || verify_code == "")
    {
        emit sign2UIMessage("The input item cannot be empty!", false);
        return;
    }

    QJsonObject json;
    json["username"] = username;
    json["password"] = new_password;
    json["email"] = email;
    json["verifycode"] = verify_code;
    // 发送重置密码请求
    HttpMgr::getInstance()->sendPostRequest(QUrl(gate_url_prefix + "/reset_password"), json, ReqId::ID_RESET_PASSWORD, Modules::RESET);
}

void ResetPage::initHttpHandler()
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
    // 重置密码后处理
    http_handlers_.insert({ReqId::ID_RESET_PASSWORD, [this](const QJsonObject &obj)
                           {
                               int erro_code = obj.value("error").toInt();
                               if (!erro_code == 0)
                               {
                                   emit sign2UIMessage("Parameter error!", false);
                                   return;
                               }
                               auto email = obj.value("email").toString();
                               emit sign2UIResetSuccess(email);
                           }});
}

void ResetPage::slotResetModuleDone(ReqId id, const std::string &res, ErrorCode code)
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