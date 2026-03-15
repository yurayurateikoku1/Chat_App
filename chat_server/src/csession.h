#pragma once
#include "common.h"
#include "msg_node.h"
#include <nlohmann/json.hpp>
#include <queue>
class CServer;
class LogicSystem;
class CSession : public std::enable_shared_from_this<CSession>
{
public:
    CSession(boost::asio::io_context &io_context, CServer *server = nullptr);
    ~CSession();

    boost::asio::ip::tcp::socket &getSocket() { return socket_; }
    std::string getUid() { return uid_; }
    void lunchSession();
    void close();
    void send(const std::string &message, short message_id);

private:
    void handleRead(const boost::system::error_code &ec, std::size_t bytes_transferred,
                    std::shared_ptr<CSession> self);
    void handleWrite(const boost::system::error_code &ec, std::size_t bytes_transferred,
                     std::shared_ptr<CSession> self);
    void asyncReadMore();
    bool parseHeader(int &copy_len, std::size_t &bytes_transferred);
    bool parseBody(int &copy_len, std::size_t &bytes_transferred);
    CServer *server_;
    boost::asio::ip::tcp::socket socket_;
    std::string uid_;
    char data_[MAX_LENGTH];
    bool flag_close_; // 是否关闭
    std::mutex mutex_;
    std::queue<std::shared_ptr<SendNode>> message_queue_; // 发送消息队列
    std::shared_ptr<RecvNode> recv_msg_node_;             // 接收节点
    bool flag_head_parse;                                 // 是否解析头
    std::shared_ptr<MsgNode> recv_head_node_;             // 消息头节点
};
