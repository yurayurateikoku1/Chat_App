#pragma once
#include <QObject>
#include <QQmlEngine>
#include "../common.h"
class LoginPage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit LoginPage(QObject *parent = nullptr);

    Q_INVOKABLE void loginUser(const QString &email, const QString &password);

signals:
    void sign2UILoginStatus(bool status);
    void sign2UIMessage(const QString &message, bool normal);

private slots:
    void handleLoginModule(ReqId id, const std::string &res, ErrorCode code);

private:
    void initHttpHandler();
    std::map<ReqId, std::function<void(const QJsonObject &)>> http_handlers_;

    int uid_ = -1;
    std::string token_;
};
