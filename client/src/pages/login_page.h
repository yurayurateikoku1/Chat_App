#pragma once
#include <QObject>
#include <QQmlEngine>

class LoginPage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit LoginPage(QObject *parent = nullptr);

    Q_INVOKABLE void loginUser(const QString &username, const QString &password);

signals:
    void signLoginSuccess();
    void signLoginFailed();
};
