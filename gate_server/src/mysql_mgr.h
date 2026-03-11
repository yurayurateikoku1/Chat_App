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

    sql::Connection *getConn();
    void returnConn(sql::Connection *conn);

private:
    std::atomic<bool> flag_stop_;
    std::size_t pool_size_;
    std::string host_;
    int port_;
    std::string username_;
    std::string passwd_;
    std::string schema_;
    std::queue<std::unique_ptr<SqlConnection>> pool_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread check_thread_;
};

class MysqlMgr
{
};