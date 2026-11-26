#pragma once
#include <pqxx/pqxx> // библа для работы с postgreSQL
#include <memory>
#include <mutex>
#include <string>

class Database {
public:
    explicit Database(const std::string& connStr);

    std::mutex&       mutex() { return mutex_; }
    pqxx::connection& conn()  { return *conn_; }

private:
    std::unique_ptr<pqxx::connection> conn_;
    std::mutex mutex_;
};