#pragma once
#include <QObject>
#include <QQmlEngine>
#include "../common.h"
class RegisterPage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit RegisterPage(QObject *parent = nullptr);

    Q_INVOKABLE void getVerifyCode(const QString &email);

private slots:
    void slotRegisterModuleDone(ReqId id, const std::string &res, ErrorCode code);
signals:
    void signUIMessage(const QString &message, bool normal);

private:
    /// @brief 初始化HttpHandler
    void initHttpHandler();

    std::map<ReqId, std::function<void(const QJsonObject &)>> http_handlers_;
};