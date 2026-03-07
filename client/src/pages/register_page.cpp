#include "register_page.h"
#include <QRegularExpression>
#include "../http_mgr.h"
RegisterPage::RegisterPage(QObject *parent)
{
    connect(HttpMgr::getInstance().get(), &HttpMgr::signRegisterModuleDone, this, &RegisterPage::slotRegisterModuleDone);
    initHttpHandler();
}

Q_INVOKABLE void RegisterPage::getVerifyCode(const QString &email)
{
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool math = regex.match(email).hasMatch();
    if (math)
    {
        /* code */
    }
    else
    {
        emit signUIMessage("The email format is incorrect!", false);
    }
}

void RegisterPage::initHttpHandler()
{
    http_handlers_.insert({ReqId::ID_GET_VERIFY_CODE, [this](const QJsonObject &obj)
                           {
                               int erro_code = obj.value("error").toInt();
                               if (!erro_code == 0)
                               {
                                   emit signUIMessage("Parameter error!", false);
                                   return;
                               }

                               auto email = obj.value("email").toString();
                               emit signUIMessage("The verification code has been sent to " + email, true);
                           }});
}

void RegisterPage::slotRegisterModuleDone(ReqId id, const std::string &res, ErrorCode code)
{
    if (code != ErrorCode::SUCCESS)
    {
        emit signUIMessage("Network request failed!", false);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(res));
    if (doc.isNull())
    {
        emit signUIMessage("Json parse failed!", false);
        return;
    }
    if (!doc.isObject())
    {
        emit signUIMessage("Json parse failed!", false);
        return;
    }

    http_handlers_[id](doc.object());
}
