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

std::shared_ptr<UserInfo> MysqlStore::getUser(int uid)
{
    auto connection = mysql_pool_->getConnection();
    if (connection == nullptr)
    {
        return nullptr;
    }

    Defer defer([this, &connection]()
                { mysql_pool_->returnConnection(std::move(connection)); });
    try
    {
        // 准备查询语句
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("SELECT * FROM user WHERE uid = ?"));
        // 设置输入参数
        ptmt->setInt(1, uid);
        // 执行查询
        std::unique_ptr<sql::ResultSet> result(ptmt->executeQuery());
        // 遍历结果集
        while (result->next())
        {
            std::shared_ptr<UserInfo> user_info = std::make_shared<UserInfo>();
            user_info->name = result->getString("name");
            user_info->email = result->getString("email");
            user_info->uid = result->getInt("uid");
            user_info->passwd = result->getString("pwd");
            user_info->nick = result->getString("nick");
            user_info->desc = result->getString("desc");
            user_info->sex = result->getInt("sex");
            user_info->icon = result->getString("icon");
            return user_info;
        }
        return nullptr;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return nullptr;
    }
}

bool MysqlStore::addFriendApply(int uid, int to_uid)
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
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("INSERT INTO friend_apply (from_uid, to_uid) VALUES (?, ?)"
                                                                                         "ON DUPLICATE KEY UPDATE from_uid = from_uid, to_uid = to_uid"));
        // 设置输入参数
        ptmt->setInt(1, uid);
        ptmt->setInt(2, to_uid);
        // 执行查询
        int result = ptmt->execute();
        if (result < 0)
        {
            return false;
        }
        return true;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return false;
    }
}

bool MysqlStore::getFriendApplyList(int to_uid, std::vector<std::shared_ptr<ApplyInfo>> &list, int begin, int limit)
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
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("select apply.from_uid, apply.status, user.name, "
                                                                                         "user.nick, user.sex, user.icon from friend_apply as apply join user on apply.from_uid = user.uid where apply.to_uid = ? "
                                                                                         "and apply.id > ? order by apply.id ASC LIMIT ? "));
        ptmt->setInt(1, to_uid);
        ptmt->setInt(2, begin);
        ptmt->setInt(3, limit);
        std::unique_ptr<sql::ResultSet> result(ptmt->executeQuery());
        while (result->next())
        {
            std::shared_ptr<ApplyInfo> apply_info = std::make_shared<ApplyInfo>();
            apply_info->uid = result->getInt("from_uid");
            apply_info->name = result->getString("name");
            apply_info->nick = result->getString("nick");
            apply_info->sex = result->getInt("sex");
            apply_info->status = result->getInt("status");
            apply_info->desc = "";
            apply_info->icon = result->getString("icon");
            list.push_back(apply_info);
        }
        return true;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return false;
    }
}

bool MysqlStore::authFriendApply(int uid, int to_uid)
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
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("update friend_apply set status = 1 where from_uid = ? and to_uid = ?"));
        ptmt->setInt(1, to_uid);
        ptmt->setInt(2, uid);
        int result = ptmt->execute();
        if (result < 0)
        {
            return false;
        }
        return true;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return false;
    }
}

bool MysqlStore::addFriend(int uid, int to_uid, const std::string &back_name)
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
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("INSERT IGNORE INTO friend (self_id, friend_id, back) VALUES (?, ?, ?)"));
        ptmt->setInt(1, uid);
        ptmt->setInt(2, to_uid);
        ptmt->setString(3, back_name);
        int result = ptmt->execute();
        if (result < 0)
        {
            return false;
        }

        std::unique_ptr<sql::PreparedStatement> ptmt1(connection->conn_->prepareStatement("INSERT IGNORE INTO friend (self_id, friend_id, back) VALUES (?, ?, ?)"));
        ptmt1->setInt(1, to_uid);
        ptmt1->setInt(2, uid);
        ptmt1->setString(3, back_name);
        int result1 = ptmt1->execute();
        if (result1 < 0)
        {
            return false;
        }

        return true;
    }
    catch (const sql::SQLException &e)
    {
        SPDLOG_ERROR("SQLException: {}", e.what());
        return false;
    }
}

bool MysqlStore::getFriendList(int uid, std::vector<std::shared_ptr<UserInfo>> &list)
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
        std::unique_ptr<sql::PreparedStatement> ptmt(connection->conn_->prepareStatement("select * from friend where self_id = ?"));
        ptmt->setInt(1, uid);
        std::unique_ptr<sql::ResultSet> result(ptmt->executeQuery());
        while (result->next())
        {
            std::shared_ptr<UserInfo> user_info = std::make_shared<UserInfo>();
            user_info->uid = result->getInt("friend_id");

            auto user_tmp = getUser(user_info->uid);
            if (user_tmp == nullptr)
            {
                continue;
            }
            user_info->name = user_tmp->name;
            user_info->nick = user_tmp->nick;
            user_info->sex = user_tmp->sex;
            user_info->desc = user_tmp->desc;
            user_info->icon = user_tmp->icon;
            list.push_back(user_info);
        }
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

bool MysqlMgr::addFriendApply(int uid, int to_uid)
{
    return mysql_store_.addFriendApply(uid, to_uid);
}

bool MysqlMgr::authFriendApply(int uid, int to_uid)
{
    return mysql_store_.authFriendApply(uid, to_uid);
}

bool MysqlMgr::addFriend(int uid, int to_uid, const std::string &back_name)
{
    return mysql_store_.addFriend(uid, to_uid, back_name);
}

std::shared_ptr<UserInfo> MysqlMgr::getUser(int uid)
{
    return mysql_store_.getUser(uid);
}

bool MysqlMgr::getFriendApplyList(int to_uid, std::vector<std::shared_ptr<ApplyInfo>> &list, int begin, int limit)
{
    return mysql_store_.getFriendApplyList(to_uid, list, begin, limit);
}

bool MysqlMgr::getFriendList(int uid, std::vector<std::shared_ptr<UserInfo>> &list)
{
    return mysql_store_.getFriendList(uid, list);
}
