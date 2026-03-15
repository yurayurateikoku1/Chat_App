#pragma once
#include <QObject>
#include <QQmlEngine>
#include "../common.h"
class ResetPage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit ResetPage(QObject *parent = nullptr);
    Q_INVOKABLE void getVerifyCode(const QString &email);
    Q_INVOKABLE void resetPassword(const QString &username, const QString &new_password, const QString &email, const QString &verify_code);
private slots:

    /// @brief 处理重置模块http响应
    void handleResetModule(ReqId id, const std::string &res, ErrorCode code);
signals:
    void sign2UIMessage(const QString &message, bool normal);
    void sign2UIResetSuccess(const QString &email);

private:
    void initHttpHandler();
    std::map<ReqId, std::function<void(const QJsonObject &)>> http_handlers_;
};