#include "../include/data_base_pool.h"


DbPool::DbPool(const std::string& conn_str, size_t pool_size) {
    for (size_t i = 0; i < pool_size; ++i) {
        PGconn* conn = PQconnectdb(conn_str.c_str());
        if (PQstatus(conn) != CONNECTION_OK) {
            std::cerr << "DB connection failed: " << PQerrorMessage(conn) << "\n";
            PQfinish(conn);
            throw std::runtime_error("DB connection failed");
        }
        pool_.push_back(conn);
    }
    std::cout << "DbPool created with " << pool_size << " connections\n";
}

DbPool::~DbPool() {
    for (PGconn* conn : pool_) {
        PQfinish(conn);
    }
}

PGconn* DbPool::acquire() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pool_.empty()) {
        throw std::runtime_error("No DB connections available");
    }
    PGconn* conn = pool_.back();
    pool_.pop_back();
    return conn;
}

void DbPool::release(PGconn* conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    pool_.push_back(conn);
}

PgResult DbPool::execParams(const std::string& sql, const std::vector<std::string>& params) {
    PGconn* conn = acquire();
    std::vector<const char*> param_values;
    for (const auto& p : params) {
        param_values.push_back(p.c_str());
    }

    PGresult* res = PQexecParams(
        conn, sql.c_str(), params.size(), nullptr,
        param_values.data(), nullptr, nullptr, 0
    );

    release(conn);
    return PgResult(res);
}