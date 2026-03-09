#pragma once
#include "singleton.h"
#include "common.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QNetworkReply>
class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    ~HttpMgr();
    void sendPostRequest(const QUrl &url, const QJsonObject &json, ReqId req_id, Modules module);

private:
    friend class Singleton<HttpMgr>;
    HttpMgr();
    QNetworkAccessManager manager_;

private slots:

    void soltHttpDone(ReqId req_id, const std::string &res, ErrorCode code, Modules module);
signals:
    void signHttpDone(ReqId id, const std::string &res, ErrorCode code, Modules module);
    void signRegisterModuleDone(ReqId id, const std::string &res, ErrorCode code);
};