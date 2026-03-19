#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "common.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <QSettings>
#include <QQuickStyle>
#include <QDir>
#include <QIcon>
#include "user_mgr.h"

int main(int argc, char *argv[])
{
    spdlog::stdout_color_mt("console");
    QGuiApplication app(argc, argv);
    // 确保 UserMgr 在主线程创建，避免 model 线程归属问题
    UserMgr::getInstance();
    QQuickStyle::setStyle("Basic");
    app.setWindowIcon(QIcon(":/assets/chat.png"));

    QString app_path = QGuiApplication::applicationDirPath();
    QString config_path = QString(QDir::toNativeSeparators(app_path + QDir::separator() + "config.ini"));
    QSettings settings(config_path, QSettings::Format::IniFormat);
    QString gate_host = settings.value("gate_server/host").toString();
    QString gate_port = settings.value("gate_server/port").toString();
    gate_url_prefix = QString("http://%1:%2").arg(gate_host).arg(gate_port);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []()
        { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("client", "Main");

    return app.exec();
}
