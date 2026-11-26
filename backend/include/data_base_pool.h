#pragma once
#include <libpq-fe.h>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <functional>

class PgResult {
public:
    explicit PgResult(PGresult* res) : res_(res) {}
    ~PgResult() { if (res_) PQclear(res_); }
    
    // Запрет копирования
    PgResult(const PgResult&) = delete;
    PgResult& operator=(const PgResult&) = delete;
    
    // Разрешаем перемещение
    PgResult(PgResult&& other) noexcept : res_(other.res_) { other.res_ = nullptr; }
    
    PGresult* get() const { return res_; }
    bool is_ok() const { return res_ && PQresultStatus(res_) == PGRES_TUPLES_OK; }
    bool is_command_ok() const { return res_ && PQresultStatus(res_) == PGRES_COMMAND_OK; }
    
private:
    PGresult* res_;
};

class DbPool {
public:
    DbPool(const std::string& conn_str, size_t pool_size = 4);
    ~DbPool();

    PgResult execParams(const std::string& sql, const std::vector<std::string>& params);

    void release(PGconn* conn);
    
    bool execTransaction(const std::function<void(PGconn*)>& fn);

    static PgResult execParamsTx(PGconn* conn, const std::string& sql,
                                const std::vector<std::string>& params);

private:
    PGconn* acquire();
    std::vector<PGconn*> pool_;
    std::mutex mutex_;
};
