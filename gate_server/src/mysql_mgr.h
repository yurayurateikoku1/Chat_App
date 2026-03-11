#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/exception.h>

class SqlConnection
{
public:
    SqlConnection(sql::Connection *conn, int64_t last_time)
        : conn_(conn), last_time_(last_time) {

          };
    std::unique_ptr<sql::Connection> conn_;
    int64_t last_time_; // 上次使用时间
};

class MysqlPool
{
public:
    MysqlPool(size_t pool_size, const std::string &host, int port, const std::string &username, const std::string &passwd,
              const std::string &schema);
    ~MysqlPool();

    /// @brief 检查连接
    void checkConnection();
    std::unique_ptr<SqlConnection> getConnection();
    void returnConnection(std::unique_ptr<SqlConnection> conn);

private:
    std::atomic<bool> flag_stop_;
    std::size_t pool_size_;
    std::string host_;
    int port_;
    std::string username_;
    std::string passwd_;
    std::string schema_; // 数据库名
    std::queue<std::unique_ptr<SqlConnection>> pool_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread check_thread_;
};

struct UserInfo
{
    std::string name;
    std::string passwd;
    int uid;
    std::string email;
};

class MysqlStore
{
public:
    MysqlStore();
    ~MysqlStore();
    int registerUser(const std::string &username, const std::string &password, const std::string &email);
    bool checkEmail(const std::string &username, const std::string &email);
    bool checkPassword(const std::string &username, const std::string &password);

private:
    std::unique_ptr<MysqlPool> mysql_pool_;
};

class MysqlMgr
{
public:
    static MysqlMgr &getInstance()
    {
        static MysqlMgr instance;
        return instance;
    }
    ~MysqlMgr();
    int registerUser(const std::string &username, const std::string &password, const std::string &email);

private:
    MysqlMgr();
    MysqlStore mysql_store_;
};