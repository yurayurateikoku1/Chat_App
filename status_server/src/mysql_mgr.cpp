#include "mysql_mgr.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include "common.h"
#include "config_mgr.h"
MysqlPool::MysqlPool(size_t pool_size, const std::string &host, int port, const std::string &username, const std::string &passwd, const std::string &schema)
    : pool_size_(pool_size), host_(host), port_(port), username_(username), passwd_(passwd), schema_(schema)
{
    try
    {
        for (size_t i = 0; i < pool_size_; i++)
        {
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            sql::Connection *conn = driver->connect(host_, username_, passwd_);
            conn->setSchema(schema_);
            // 获取当前时间戳
            auto now = std::chrono::system_clock::now().time_since_epoch();
            long long last_time = std::chrono::duration_cast<std::chrono::seconds>(now).count();
            pool_.push(std::make_unique<SqlConnection>(conn, last_time));
        }

        check_thread_ = std::thread([this]
                                    {
                                    while (!flag_stop_)
                                    {
                                        checkConnection();
                                        std::this_thread::sleep_for(std::chrono::seconds(1));
                                    } });
        // 线程分离
        check_thread_.detach();
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
    }
}

MysqlPool::~MysqlPool()
{
    std::lock_guard<std::mutex> lock(mutex_);
    flag_stop_ = true;
    cv_.notify_all();
    while (!pool_.empty())
    {
        pool_.pop();
    }
    check_thread_.join();
}

void MysqlPool::checkConnection()
{
    std::lock_guard<std::mutex> lock(mutex_);
    // 拷贝当前队列大小
    int pool_size = pool_.size();
    auto now = std::chrono::system_clock::now().time_since_epoch();
    long long last_time = std::chrono::duration_cast<std::chrono::seconds>(now).count();

    for (size_t i = 0; i < pool_size; i++)
    {
        // 取出头部
        auto conn = std::move(pool_.front());
        pool_.pop();

        Defer defer([this, &conn]()
                    { pool_.push(std::move(conn)); });

        // 如果操作时间小于5秒,直接跳过
        if (last_time - conn->last_time_ < 5)
        {
            continue;
        }

        try
        {

            std::unique_ptr<sql::Statement> stmt(conn->conn_->createStatement());
            std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1"));
            while (res->next())
            {
            }
            while (stmt->getMoreResults())
            {
                std::unique_ptr<sql::ResultSet> tmp(stmt->getResultSet());
                while (tmp && tmp->next())
                {
                }
            }
            conn->last_time_ = last_time;
        }
        catch (const sql::SQLException &e)
        {
            SPDLOG_ERROR("SQLException: {}", e.what());
            // 重新创建连接并替换旧连接
            sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
            sql::Connection *new_conn = driver->connect(host_, username_, passwd_);
            new_conn->setSchema(schema_);
            conn->conn_.reset(new_conn);
            conn->last_time_ = last_time;
        }
    }
}

std::unique_ptr<SqlConnection> MysqlPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]
             { return !pool_.empty() || flag_stop_; });
    if (flag_stop_)
    {
        return nullptr;
    }
    std::unique_ptr<SqlConnection> conn(std::move(pool_.front()));
    pool_.pop();
    return conn;
}

void MysqlPool::returnConnection(std::unique_ptr<SqlConnection> conn)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (flag_stop_)
    {
        return;
    }
    pool_.push(std::move(conn));
    cv_.notify_one();
}

MysqlStore::MysqlStore()
{
    auto &config_mgr = ConfigMgr::getInstance();
    auto host = config_mgr["mysql"]["host"];
    auto port = config_mgr["mysql"]["port"];
    auto username = config_mgr["mysql"]["user"];
    auto passwd = config_mgr["mysql"]["pass"];
    auto schema = config_mgr["mysql"]["schema"];
    mysql_pool_.reset(new MysqlPool(5, host, atoi(port.c_str()), username, passwd, schema));
}

MysqlStore::~MysqlStore()
{
}

int MysqlStore::registerUser(const std::string &username, const std::string &password, const std::string &email)
{
    auto connection = mysql_pool_->getConnection();
    if (connection == nullptr)
    {
        return -1;
    }
    Defer defer([this, &connection]()
                { mysql_pool_->returnConnection(std::move(connection)); });
    try
    {
        // 准备调用存储过程
        std::unique_ptr<sql::PreparedStatement> stmt(connection->conn_->prepareStatement("CALL reg_user(?, ?, ?,@result)"));
        // 设置输入参数
        stmt->setString(1, username);
        stmt->setString(2, email);
        stmt->setString(3, password);
        stmt->execute();
        // 消费存储过程产生的所有结果集，避免 "Commands out of sync" 错误
        while (stmt->getMoreResults())
        {
        }
        // 获取输出参数
        std::unique_ptr<sql::Statement> stmt_result(connection->conn_->createStatement());
        std::unique_ptr<sql::ResultSet> result(stmt_result->executeQuery("SELECT @result AS result"));
        if (result->next())
        {
            int result_code = result->getInt("result");
            SPDLOG_INFO("Register user result: {}", result_code);
            return result_code;
        }
        return -1;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return -1;
    }
}

bool MysqlStore::checkEmail(const std::string &username, const std::string &email)
{
    auto connection = mysql_pool_->getConnection();
    if (connection == nullptr)
    {
        return false;
    }

    Defer defer([this, &connection]()
                { mysql_pool_->returnConnection(std::move(connection)); });

    try
    {
        // 准备查询语句
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("SELECT email FROM user WHERE name = ?"));
        // 设置输入参数
        ptmt->setString(1, username);
        // 执行查询
        std::unique_ptr<sql::ResultSet> result(ptmt->executeQuery());
        // 遍历结果集
        while (result->next())
        {
            std::string result_email = result->getString("email");
            if (result_email != email)
            {
                return false;
            }
            return true;
        }
        return true;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return false;
    }
}

bool MysqlStore::updatePassword(const std::string &username, const std::string &password)
{
    auto connection = mysql_pool_->getConnection();
    if (connection == nullptr)
    {
        return false;
    }

    Defer defer([this, &connection]()
                { mysql_pool_->returnConnection(std::move(connection)); });

    try
    {
        // 准备查询语句
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
        // 设置输入参数
        ptmt->setString(1, password);
        ptmt->setString(2, username);
        // 执行查询
        ptmt->execute();
        return true;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return false;
    }
}

bool MysqlStore::checkPassword(const std::string &email, const std::string &password, UserInfo &user_info)
{
    auto connection = mysql_pool_->getConnection();
    if (connection == nullptr)
    {
        return false;
    }

    Defer defer([this, &connection]()
                { mysql_pool_->returnConnection(std::move(connection)); });
    try
    {
        // 准备查询语句
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("SELECT * FROM user WHERE email = ?"));
        // 设置输入参数
        ptmt->setString(1, email);
        // 执行查询
        std::unique_ptr<sql::ResultSet> result(ptmt->executeQuery());
        // 遍历结果集
        std::string origin_password = "";
        while (result->next())
        {
            origin_password = result->getString("pwd");
            break;
        }

        if (origin_password != password)
        {
            return false;
        }

        user_info.name = result->getString("name");
        user_info.email = email;
        user_info.uid = result->getInt("uid");
        user_info.passwd = origin_password;
        return true;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return false;
    }
}

MysqlMgr::MysqlMgr()
{
}

MysqlMgr::~MysqlMgr()
{
}

int MysqlMgr::registerUser(const std::string &username, const std::string &password, const std::string &email)
{
    return mysql_store_.registerUser(username, password, email);
}

bool MysqlMgr::checkEmail(const std::string &username, const std::string &email)
{
    return mysql_store_.checkEmail(username, email);
}

bool MysqlMgr::checkPassword(const std::string &email, const std::string &password, UserInfo &user_info)
{
    return mysql_store_.checkPassword(email, password, user_info);
}

bool MysqlMgr::updatePassword(const std::string &username, const std::string &password)
{
    return mysql_store_.updatePassword(username, password);
}
