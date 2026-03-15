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
    Q_INVOKABLE void registerUser(const QString &username, const QString &password, const QString &confirm_password,
                                  const QString &email, const QString &verify_code);
private slots:

    /// @brief 处理注册模块http响应
    void handleRegisterModule(ReqId id, const std::string &res, ErrorCode code);
signals:
    void sign2UIMessage(const QString &message, bool normal);
    void sign2UIRegisterSuccess(const QString &email);

private:
    /// @brief 初始化HttpHandler,封装http响应回调后处理
    void initHttpHandler();

    std::map<ReqId, std::function<void(const QJsonObject &)>> http_handlers_;
};