#include "redis_mgr.h"
#include "config_mgr.h"
#include "common.h"
#include <spdlog/spdlog.h>

RedisPool::RedisPool(size_t pool_size, const std::string &host, int port, const std::string &passwd)
    : pool_size_(pool_size), host_(host), port_(port), passwd_(passwd)
{
    for (size_t i = 0; i < pool_size_; i++)
    {
        redisContext *ctx = redisConnect(host.c_str(), port);
        if (ctx == nullptr && ctx->err != 0)
        {
            if (ctx != nullptr)
            {
                redisFree(ctx);
            }
            continue;
        }

        auto reply = (redisReply *)redisCommand(ctx, "AUTH %s", passwd.c_str());
        if (reply->type == REDIS_REPLY_ERROR)
        {
            SPDLOG_ERROR("redis auth error:{}", ctx->errstr);
            redisFree(ctx);
            continue;
        }

        freeReplyObject(reply);
        SPDLOG_INFO("redis auth success");
        pool_.push(ctx);
    }
}

RedisPool::~RedisPool()
{
    std::lock_guard<std::mutex> lock(mutex_);
    flag_stop_ = true;
    cv_.notify_all();
    while (!pool_.empty())
    {
        pool_.pop();
    }
}

redisContext *RedisPool::getRedisContext()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]
             { return !pool_.empty() || flag_stop_; });

    if (flag_stop_)
    {
        return nullptr;
    }
    else
    {
        redisContext *ctx = pool_.front();
        pool_.pop();
        return ctx;
    }
}

void RedisPool::returnRedisContext(redisContext *ctx)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (flag_stop_)
    {
        return;
    }
    pool_.push(ctx);
    cv_.notify_one();
}

RedisMgr::RedisMgr()
{
    auto &config_mgr = ConfigMgr::getInstance();
    auto host = config_mgr["redis"]["host"];
    auto port = config_mgr["redis"]["port"];
    auto passwd = config_mgr["redis"]["pass"];

    redis_pool_.reset(new RedisPool(5, host, atoi(port.c_str()), passwd));
}

RedisMgr::~RedisMgr()
{
}

bool RedisMgr::getValue(const std::string &key, std::string &value)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });

    auto reply = (redisReply *)redisCommand(ctx, "GET %s", key.c_str());
    if (reply == nullptr)
    {
        SPDLOG_ERROR("redis get error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    if (reply->type != REDIS_REPLY_STRING)
    {
        SPDLOG_ERROR("redis get error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    value = reply->str;
    SPDLOG_INFO("redis get key:{} value:{}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::setValue(const std::string &key, const std::string &value)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });

    auto reply = (redisReply *)redisCommand(ctx, "SET %s %s", key.c_str(), value.c_str());
    if (reply == nullptr)
    {
        SPDLOG_ERROR("redis set error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0)))
    {
        SPDLOG_ERROR("redis set error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("redis set key:{} value:{}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::auth(const std::string &passwd)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "AUTH %s", passwd.c_str());
    if (reply->type == REDIS_REPLY_ERROR)
    {
        SPDLOG_ERROR("redis auth error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("redis auth success");
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::lpushValue(const std::string &key, const std::string &value)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "LPUSH %s %s", key.c_str(), value.c_str());
    if (reply == nullptr)
    {
        SPDLOG_ERROR("redis lpush error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER || reply->integer < 0)
    {
        SPDLOG_ERROR("redis lpush error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("redis lpush key:{} value:{}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::lpopValue(const std::string &key, std::string &value)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "LPOP %s", key.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL)
    {
        SPDLOG_ERROR("redis lpop error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    value = reply->str;
    SPDLOG_INFO("redis lpop key:{} value:{}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::rpopValue(const std::string &key, std::string &value)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "RPOP %s", key.c_str());
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL)
    {
        SPDLOG_ERROR("redis rpop error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    value = reply->str;
    SPDLOG_INFO("redis rpop key:{} value:{}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::rpushValue(const std::string &key, const std::string &value)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "RPUSH %s %s", key.c_str(), value.c_str());
    if (reply == nullptr)
    {
        SPDLOG_ERROR("redis rpush error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER || reply->integer < 0)
    {
        SPDLOG_ERROR("redis rpush error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("redis rpush key:{} value:{}", key, value);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::hsetValue(const std::string &key, const std::string &field, const std::string &value)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (reply == nullptr)
    {
        SPDLOG_ERROR("redis hset error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER || reply->integer < 0)
    {
        SPDLOG_ERROR("redis hset error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("redis hset key:{} field:{} value:{}", key, field, value);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::hsetValue(const char *key, const char *field, const char *value, size_t len)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    const char *args[4];
    size_t arglen[4];
    args[0] = "HSET";
    arglen[0] = strlen(args[0]);
    args[1] = key;
    arglen[1] = strlen(key);
    args[2] = field;
    arglen[2] = strlen(field);
    args[3] = value;
    arglen[3] = len;
    auto reply = (redisReply *)redisCommandArgv(ctx, 4, args, arglen);
    if (reply == nullptr)
    {
        SPDLOG_ERROR("redis hset error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER || reply->integer < 0)
    {
        SPDLOG_ERROR("redis hset error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("redis hset key:{} field:{} value:{}", key, field, value);
    freeReplyObject(reply);
    return true;
}

std::string RedisMgr::hgetValue(const std::string &key, const std::string &field)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return "";
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    const char *args[3];
    size_t arglen[3];
    args[0] = "HGET";
    arglen[0] = strlen(args[0]);
    args[1] = key.c_str();
    arglen[1] = key.length();
    args[2] = field.c_str();
    arglen[2] = field.length();
    auto reply = (redisReply *)redisCommandArgv(ctx, 3, args, arglen);
    if (reply == nullptr || reply->type == REDIS_REPLY_NIL)
    {
        SPDLOG_ERROR("redis hget error:{}", ctx->errstr);
        freeReplyObject(reply);
        return "";
    }

    std::string value = reply->str;
    SPDLOG_INFO("redis hget key:{} field:{} value:{}", key, field, value);
    freeReplyObject(reply);
    return value;
}

bool RedisMgr::delValue(const std::string &key)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "DEL %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER)
    {
        SPDLOG_ERROR("redis del error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }

    SPDLOG_INFO("redis del key:{}", key);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::hdelValue(const std::string &key, const std::string &field)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "HDEL %s %s", key.c_str(), field.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
    {
        SPDLOG_ERROR("redis hdel error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("redis hdel key:{} field:{}", key, field);
    freeReplyObject(reply);
    return true;
}

bool RedisMgr::existsValue(const std::string &key)
{
    auto ctx = redis_pool_->getRedisContext();
    if (ctx == nullptr)
    {
        return false;
    }
    Defer defer([this, ctx]() { redis_pool_->returnRedisContext(ctx); });
    auto reply = (redisReply *)redisCommand(ctx, "EXISTS %s", key.c_str());
    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
    {
        SPDLOG_ERROR("redis exists error:{}", ctx->errstr);
        freeReplyObject(reply);
        return false;
    }
    SPDLOG_INFO("redis exists key:{}", key);
    freeReplyObject(reply);
    return true;
}
