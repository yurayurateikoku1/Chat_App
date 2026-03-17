#include "csession.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <spdlog/spdlog.h>
#include "cserver.h"
#include "logic_system.h"

CSession::CSession(boost::asio::io_context &io_context, CServer *server)
    : socket_(io_context), server_(server), flag_close_(false), flag_head_parse(false)
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    uid_ = boost::uuids::to_string(uuid);
    recv_head_node_ = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession()
{
}

void CSession::lunchSession()
{
    asyncReadMore();
}

void CSession::close()
{
    socket_.close();
    flag_close_ = true;
}

void CSession::send(const std::string &message, short message_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    int queue_len = message_queue_.size();
    if (queue_len >= MAX_SENDQUE)
    {
        SPDLOG_ERROR("message queue full");
        return;
    }
    message_queue_.push(std::make_shared<SendNode>(message.c_str(), message.length(), message_id));
    if (queue_len > 0)
    {
        return;
    }

    auto &msg_node = message_queue_.front();
    auto self = shared_from_this();
    socket_.async_write_some(boost::asio::buffer(msg_node->buf_, msg_node->total_len_), [self, msg_node](boost::system::error_code ec, std::size_t bytes_transferred)
                             { self->handleWrite(ec, bytes_transferred, self); });
}

// 发起下一次异步读取
void CSession::asyncReadMore()
{
    memset(data_, 0, MAX_LENGTH);
    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(data_, MAX_LENGTH),
                            [self](boost::system::error_code ec, std::size_t bytes_transferred)
                            { self->handleRead(ec, bytes_transferred, self); });
}

// 解析头部，返回 true 表示头部解析完成，false 表示数据不足需等待
bool CSession::parseHeader(int &copy_len, std::size_t &bytes_transferred)
{
    // 数据不够一个完整头部，缓存后等待更多数据
    if (bytes_transferred + recv_head_node_->current_len_ < HEAD_TOTAL_LEN)
    {
        memcpy(recv_head_node_->buf_ + recv_head_node_->current_len_, data_ + copy_len, bytes_transferred);
        recv_head_node_->current_len_ += bytes_transferred;
        bytes_transferred = 0;
        return false;
    }

    // 补齐头部剩余部分
    int head_remain = HEAD_TOTAL_LEN - recv_head_node_->current_len_;
    memcpy(recv_head_node_->buf_ + recv_head_node_->current_len_, data_ + copy_len, head_remain);
    copy_len += head_remain;
    bytes_transferred -= head_remain;

    // 解析 msg_id
    short msg_id = 0;
    memcpy(&msg_id, recv_head_node_->buf_, HEAD_ID_LEN);
    msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
    if (msg_id > MAX_LENGTH)
    {
        SPDLOG_ERROR("message id error");
        server_->clearSession(uid_);
        return false;
    }

    // 解析 msg_len
    short msg_len = 0;
    memcpy(&msg_len, recv_head_node_->buf_ + HEAD_ID_LEN, HEAD_DATA_LEN);
    msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
    if (msg_len > MAX_LENGTH)
    {
        SPDLOG_ERROR("message len error");
        server_->clearSession(uid_);
        return false;
    }

    SPDLOG_INFO("Parsed header: msg_id={}, msg_len={}", msg_id, msg_len);
    recv_msg_node_ = std::make_shared<RecvNode>(msg_len, msg_id);
    flag_head_parse = true;
    return true;
}

// 解析数据体，返回 true 表示消息完整可派发，false 表示数据不足需等待
bool CSession::parseBody(int &copy_len, std::size_t &bytes_transferred)
{
    int remain = recv_msg_node_->total_len_ - recv_msg_node_->current_len_;

    if (static_cast<int>(bytes_transferred) < remain)
    {
        // 数据不足，缓存已有部分
        memcpy(recv_msg_node_->buf_ + recv_msg_node_->current_len_, data_ + copy_len, bytes_transferred);
        recv_msg_node_->current_len_ += bytes_transferred;
        bytes_transferred = 0;
        return false;
    }

    // 数据体完整
    memcpy(recv_msg_node_->buf_ + recv_msg_node_->current_len_, data_ + copy_len, remain);
    recv_msg_node_->current_len_ += remain;
    copy_len += remain;
    bytes_transferred -= remain;
    recv_msg_node_->buf_[recv_msg_node_->total_len_] = '\0';
    return true;
}

// 每条消息由 [msg_id (2字节)] [msg_len (2字节)] [数据体] 组成
void CSession::handleRead(const boost::system::error_code &ec, std::size_t bytes_transferred, std::shared_ptr<CSession> self)
{
    try
    {
        if (ec)
        {
            SPDLOG_ERROR(ec.message());
            close();
            server_->clearSession(uid_);
            return;
        }

        int copy_len = 0;

        while (bytes_transferred > 0)
        {
            // 步骤1：如果头部未解析完，先解析头部
            if (!flag_head_parse)
            {
                if (!parseHeader(copy_len, bytes_transferred))
                {
                    asyncReadMore();
                    return;
                }
            }

            // 步骤2：解析数据体
            if (!parseBody(copy_len, bytes_transferred))
            {
                asyncReadMore();
                return;
            }

            // 步骤3：消息完整，派发并重置状态
            LogicSystem::getInstance()->postMessage2Queue(
                std::make_shared<LogicNode>(shared_from_this(), recv_msg_node_));
            flag_head_parse = false;
            recv_head_node_->clear();
        }

        // 本次数据全部处理完，继续读取
        asyncReadMore();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR(e.what());
    }
}

void CSession::handleWrite(const boost::system::error_code &ec, std::size_t bytes_transferred, std::shared_ptr<CSession> self)
{
    try
    {
        if (!ec)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            message_queue_.pop();
            if (!message_queue_.empty())
            {
                auto &msg_node = message_queue_.front();
                socket_.async_write_some(boost::asio::buffer(msg_node->buf_, msg_node->total_len_), [self, msg_node](boost::system::error_code ec, std::size_t bytes_transferred)
                                         { self->handleWrite(ec, bytes_transferred, self); });
            }
        }
        else
        {
            SPDLOG_ERROR(ec.message());
            close();
            server_->clearSession(uid_);
        }
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR(e.what());
    }
}
