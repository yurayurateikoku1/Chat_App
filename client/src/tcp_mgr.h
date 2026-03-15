#pragma once
#include "singleton.h"
#include <QTCPSocket>
#include "common.h"
#include <QObject>
#include <QThread>
#include <functional>

/// @brief TCP 网络管理
class TCPMgr : public QObject, public Singleton<TCPMgr>, public std::enable_shared_from_this<TCPMgr>
{
    Q_OBJECT
    friend class Singleton<TCPMgr>;

public:
    ~TCPMgr();

private:
    TCPMgr();
    void initHttpHandler();
    void initSocket();
    std::map<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> http_handlers_;
    QTcpSocket *socket_ = nullptr;
    QThread worker_thread_;
    std::string host_;
    int port_;
    QByteArray buffer_;
    bool flag_recv_pending_ = false;
    quint16 data_len_ = 0;
    quint16 data_id_;
public slots:
    /// @brief TCP长连接
    void tcpConnection(const ServerInfo &info);

    /// @brief 发送数据
    void tcpSendData(const ReqId &id, const QString &data);

signals:

    /// @brief 发送TCP连接状态
    /// @param status
    void signTcpConnectionStatus(bool status);

    /// @brief 发送聊天登录状态
    /// @param status
    void signLoginChatStatus(bool status);
};
