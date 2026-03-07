#include "http_mgr.h"

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::signHttpDone, this, &HttpMgr::soltHttpDone);
}

void HttpMgr::httpPostRequest(const QUrl &url, const QJsonObject &json, ReqId req_id, Modules module)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));

    auto self = shared_from_this();
    QNetworkReply *reply = manager_.post(req, data);
    connect(reply, &QNetworkReply::finished, [self, reply, req_id, module]
            {
                // 错误处理
                if (reply->error() != QNetworkReply::NoError)
                {
                    emit self->signHttpDone(req_id, "", ErrorCode::ERROR_NETWORK, module);
                    reply->deleteLater();
                    return;
                } 
                   
                std::string res = reply->readAll().toStdString();
                emit self->signHttpDone(req_id, res, ErrorCode::SUCCESS, module);
                reply->deleteLater(); });
}

void HttpMgr::soltHttpDone(ReqId req_id, const std::string &res, ErrorCode code, Modules module)
{
    switch (module)
    {
    case Modules::REGISTER:
    {
        emit signRegisterModuleDone(req_id, res, code);
        break;
    }

    default:
        break;
    }
}

HttpMgr::~HttpMgr()
{
}
