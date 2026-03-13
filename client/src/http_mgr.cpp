#include "http_mgr.h"

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::signHttpDone, this, &HttpMgr::soltHttpDone);
}

void HttpMgr::sendPostRequest(const QUrl &url, const QJsonObject &json, ReqId req_id, Modules module)
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
                    //发送信号通知收到响应
                    emit self->signHttpDone(req_id, "", ErrorCode::RPCFAILED, module);
                    reply->deleteLater();
                    return;
                } 
                   
                std::string res = reply->readAll().toStdString();
                //发送信号通知收到响应
                emit self->signHttpDone(req_id, res, ErrorCode::SUCCESS, module);
                reply->deleteLater(); });
}

void HttpMgr::soltHttpDone(ReqId req_id, const std::string &res, ErrorCode code, Modules module)
{
    // 分发响应到各个模块
    switch (module)
    {
    case Modules::REGISTER:
    {
        // 发送信号到注册模块
        emit signRegisterModuleDone(req_id, res, code);
        break;
    }
    case Modules::RESET:
    {
        // 发送信号到重置密码模块
        emit signResetModuleDone(req_id, res, code);
        break;
    }
    case Modules::LOGIN:
    {
        // 发送信号到登录模块
        emit signLoginModuleDone(req_id, res, code);
        break;
    }
    default:
        break;
    }
}

HttpMgr::~HttpMgr()
{
}
