#pragma once
#include <thread>
#include <memory>
#include <boost/asio.hpp>
#include "singleton.h"

/// @brief IOContext 线程池，每一个线程都有自己的 io_context
class IOContextPool : public Singleton<IOContextPool>
{
    friend class Singleton<IOContextPool>;

public:
    IOContextPool(const IOContextPool &) = delete;
    IOContextPool &operator=(const IOContextPool &) = delete;
    ~IOContextPool();

    boost::asio::io_context &getIOContext();
    void stop();

private:
    explicit IOContextPool(std::size_t thread_num = 2);
    std::vector<boost::asio::io_context> io_contexts_;
    // 每个 io_context 配一个 work guard，确保即使当前没有异步操作，run() 也会持续阻塞等待新任务，而不是立即返回。
    std::vector<std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>> work_guards_;
    std::vector<std::thread> threads_;
    std::size_t next_io_context = 0;
};