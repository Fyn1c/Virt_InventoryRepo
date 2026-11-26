#include "db.hpp"

Database::Database(const std::string& connStr)
    : conn_(std::make_unique<pqxx::connection>(connStr)) {
    if (!conn_->is_open()) {
        throw std::runtime_error("DB connection failed");
    }
}