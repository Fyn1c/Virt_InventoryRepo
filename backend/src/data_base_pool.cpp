#include "../include/data_base_pool.h"
#include <functional>


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


PgResult DbPool::execParamsTx(PGconn* conn, const std::string& sql,
                              const std::vector<std::string>& params) {
    std::vector<const char*> param_values;
    param_values.reserve(params.size());
    for (const auto& p : params) param_values.push_back(p.c_str());

    PGresult* res = PQexecParams(
        conn, sql.c_str(), params.size(), nullptr,
        param_values.data(), nullptr, nullptr, 0
    );
    return PgResult(res);
}

bool DbPool::execTransaction(const std::function<void(PGconn*)>& fn) {
    PGconn* conn = acquire();
    struct Releaser {
        DbPool* pool; PGconn* conn;
        ~Releaser() { pool->release(conn); }
    } releaser{this, conn};

    auto exec = [&](const std::string& sql) {
        auto r = execParamsTx(conn, sql, {});
        if (!r.is_command_ok()) {
            throw std::runtime_error(std::string("tx exec failed: ") +
                                     PQerrorMessage(conn));
        }
    };

    try {
        exec("BEGIN");
        fn(conn);
        exec("COMMIT");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Transaction rollback: " << e.what() << "\n";
        auto r = execParamsTx(conn, "ROLLBACK", {});
        (void)r;
        return false;
    }
}