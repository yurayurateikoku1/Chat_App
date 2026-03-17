#pragma once
#include <boost/asio.hpp>
#include "common.h"
class LogicSystem;
class CSession;
/// @brief 消息节点
struct MsgNode
{
    MsgNode(short max_len)
        : total_len_(max_len), current_len_(0)
    {
        buf_ = new char[total_len_ + 1];
        buf_[total_len_] = '\0';
    }
    ~MsgNode()
    {
        delete[] buf_;
    }
    void clear()
    {
        std::memset(buf_, 0, total_len_);
        current_len_ = 0;
    }
    char *buf_;
    short total_len_;
    short current_len_;
};

/// @brief 接收节点
struct RecvNode : public MsgNode
{
    friend class LogicSystem;
    RecvNode(short max_len, short message_id)
        : MsgNode(max_len), message_id_(message_id)
    {
    }

private:
    short message_id_;
};

/// @brief 发送节点
struct SendNode : public MsgNode
{
    friend class LogicSystem;
    SendNode(const char *buf, short max_len, short message_id)
        : MsgNode(max_len + HEAD_TOTAL_LEN), message_id_(message_id)
    {
        // 先拷贝消息头
        short msg_id_host = boost::asio::detail::socket_ops::host_to_network_short(message_id);
        std::memcpy(buf_, &msg_id_host, HEAD_ID_LEN);

        // 再拷贝消息体
        short msg_len_host = boost::asio::detail::socket_ops::host_to_network_short(max_len);
        std::memcpy(buf_ + HEAD_ID_LEN, &msg_len_host, HEAD_DATA_LEN);
        // 再拷贝消息
        std::memcpy(buf_ + HEAD_ID_LEN + HEAD_DATA_LEN, buf, max_len);
    }

private:
    short message_id_;
};

/// @brief 逻辑节点
struct LogicNode
{
    friend class LogicSystem;
    LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recv_node)
        : session_(session), recv_node_(recv_node) {};

private:
    std::shared_ptr<CSession> session_;
    std::shared_ptr<RecvNode> recv_node_;
};