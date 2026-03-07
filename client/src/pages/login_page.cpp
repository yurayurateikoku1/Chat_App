#include "login_page.h"
#include <QDebug>

LoginPage::LoginPage(QObject *parent) : QObject(parent) {}

void LoginPage::loginUser(const QString &username, const QString &password)
{
    qDebug() << "Login:" << username;
    // TODO: 实际登录逻辑
}
