#include "iocontext_pool.h"
#include <spdlog/spdlog.h>

IOContextPool::IOContextPool(std::size_t thread_num)
    : io_contexts_(thread_num), work_guards_(thread_num), next_io_context(0)
{
    for (size_t i = 0; i < thread_num; i++)
    {
        work_guards_[i] = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(boost::asio::make_work_guard(io_contexts_[i]));
    }

    for (size_t i = 0; i < thread_num; i++)
    {
        threads_.emplace_back([this, i]
                              { io_contexts_[i].run(); });
    }
}

IOContextPool::~IOContextPool()
{
    stop();
    SPDLOG_INFO("IOContextPool exit");
}

boost::asio::io_context &IOContextPool::getIOContext()
{
    auto &io_context = io_contexts_[next_io_context++];
    if (next_io_context == io_contexts_.size())
    {
        next_io_context = 0;
    }
    return io_context;
}

void IOContextPool::stop()
{
    for (auto &guard : work_guards_)
    {
        guard->reset();
    }
    for (auto &io_context : io_contexts_)
    {
        io_context.stop();
    }

    for (auto &thread : threads_)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}
