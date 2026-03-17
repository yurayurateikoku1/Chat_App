#pragma once
#include <string>
#include <hiredis/hiredis.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
/// @brief redis连接池
class RedisPool
{
public:
    RedisPool(size_t pool_size, const std::string &host, int port, const std::string &passwd);
    ~RedisPool();
    redisContext *getRedisContext();
    void returnRedisContext(redisContext *ctx);

private:
    size_t pool_size_;
    std::string host_;
    int port_;
    std::string passwd_;
    redisContext *ctx_;
    std::queue<redisContext *> pool_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> flag_stop_;
};

/// @brief redis操作
class RedisMgr
{
public:
    ~RedisMgr();
    static RedisMgr &getInstance()
    {
        static RedisMgr instance;
        return instance;
    }
    bool getValue(const std::string &key, std::string &value);
    bool setValue(const std::string &key, const std::string &value);
    bool auth(const std::string &passwd);
    bool lpushValue(const std::string &key, const std::string &value);
    bool lpopValue(const std::string &key, std::string &value);
    bool rpopValue(const std::string &key, std::string &value);
    bool rpushValue(const std::string &key, const std::string &value);
    bool hsetValue(const std::string &key, const std::string &field, const std::string &value);
    bool hsetValue(const char *key, const char *field, const char *value, size_t len);
    std::string hgetValue(const std::string &key, const std::string &field);
    bool delValue(const std::string &key);
    bool existsValue(const std::string &key);

private:
    RedisMgr();

    std::unique_ptr<RedisPool> redis_pool_;
};