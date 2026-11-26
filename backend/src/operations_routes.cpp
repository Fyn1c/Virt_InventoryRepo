#include "../include/operations_routes.hpp"
#include "../include/errors.h"
#include "../include/validation.h"
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

namespace {

HttpResponse from_error(const ApiError& e) {
    return { e.http_status, "application/json; charset=utf-8", e.to_json().dump() };
}

std::int64_t computeStockOn(PGconn* conn, std::int64_t productId) {
    const char* sql =
        "SELECT COALESCE(SUM(CASE WHEN type='приход' THEN quantity "
        "                        WHEN type='расход' THEN -quantity "
        "                        ELSE 0 END), 0) "
        "FROM operations WHERE product_id = $1";
    auto r = DbPool::execParamsTx(conn, sql, { std::to_string(productId) });
    if (!r.is_ok() || PQntuples(r.get()) == 0)
        throw std::runtime_error("stock query failed");
    return std::stoll(PQgetvalue(r.get(), 0, 0));
}

json operation_to_json(PGresult* r, int row = 0) {
    json item;
    item["id"]          = std::stoi(PQgetvalue(r, row, 0));
    item["productId"]   = std::stoi(PQgetvalue(r, row, 1));
    item["productName"] = PQgetvalue(r, row, 2);
    item["type"]        = PQgetvalue(r, row, 3);
    item["quantity"]    = std::stoi(PQgetvalue(r, row, 4));
    if (PQgetisnull(r, row, 5))
        item["resultStatus"] = nullptr;
    else
        item["resultStatus"] = PQgetvalue(r, row, 5);
    item["createdAt"]   = PQgetvalue(r, row, 6);
    return item;
}

}  // namespace

// POST /api/products/{id}/operations
HttpResponse handle_create_operation(DbPool& db, int product_id,
                                     const std::string& raw_body) {
    auto body = nlohmann::json::parse(raw_body, nullptr, false);
    if (body.is_discarded())
        return from_error(ApiError::validation_error({{"body", "invalid JSON"}}));

    auto v = validation::validateOperationCreate(body);
    if (!v.result.ok())
        return from_error(ApiError::validation_error(v.result.errors));

    const auto& req = v.request;

    // Товар существует?
    auto exists = db.execParams("SELECT 1 FROM products WHERE id = $1",
                                { std::to_string(product_id) });
    if (!exists.is_ok())
        return from_error(ApiError::internal("DB error"));
    if (PQntuples(exists.get()) == 0)
        return from_error(ApiError::not_found("Товар не найден"));

    HttpResponse response = from_error(ApiError::internal("transaction failed"));
    bool committed = false;

    try {
        committed = db.execTransaction([&](PGconn* conn) {
            std::string type_str = validation::toString(req.type);
            std::string qty_str  = std::to_string(req.quantity);

            if (req.type == validation::OperationType::Expense) {
                // блокируем строку товара
                auto lock = DbPool::execParamsTx(conn,
                    "SELECT id FROM products WHERE id = $1 FOR UPDATE",
                    { std::to_string(product_id) });
                if (!lock.is_ok() || PQntuples(lock.get()) == 0)
                    throw std::runtime_error("product lock failed");

                std::int64_t stock = computeStockOn(conn, product_id);
                if (req.quantity > stock)
                    throw std::runtime_error("INSUFFICIENT_STOCK");

                auto ins = DbPool::execParamsTx(conn,
                    "INSERT INTO operations (product_id, type, quantity, result_status) "
                    "VALUES ($1, $2, $3, NULL) "
                    "RETURNING id, product_id, "
                    "  (SELECT name FROM products WHERE id = $1), "
                    "  type, quantity, result_status, "
                    "  to_char(created_at AT TIME ZONE 'UTC', "
                    "          'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"')",
                    { std::to_string(product_id), type_str, qty_str });
                if (!ins.is_ok()) throw std::runtime_error("insert failed");

                response = { 201, "application/json; charset=utf-8",
                             operation_to_json(ins.get()).dump() };
            }
            else if (req.type == validation::OperationType::Income) {
                auto ins = DbPool::execParamsTx(conn,
                    "INSERT INTO operations (product_id, type, quantity, result_status) "
                    "VALUES ($1, $2, $3, NULL) "
                    "RETURNING id, product_id, "
                    "  (SELECT name FROM products WHERE id = $1), "
                    "  type, quantity, result_status, "
                    "  to_char(created_at AT TIME ZONE 'UTC', "
                    "          'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"')",
                    { std::to_string(product_id), type_str, qty_str });
                if (!ins.is_ok()) throw std::runtime_error("insert failed");

                response = { 201, "application/json; charset=utf-8",
                             operation_to_json(ins.get()).dump() };
            }
            else {  // Move
                const std::string& target = *req.targetStatus;

                auto upd = DbPool::execParamsTx(conn,
                    "UPDATE products SET status = $1 WHERE id = $2",
                    { target, std::to_string(product_id) });
                if (!upd.is_command_ok()) throw std::runtime_error("update failed");

                auto ins = DbPool::execParamsTx(conn,
                    "INSERT INTO operations (product_id, type, quantity, result_status) "
                    "VALUES ($1, $2, $3, $4) "
                    "RETURNING id, product_id, "
                    "  (SELECT name FROM products WHERE id = $1), "
                    "  type, quantity, result_status, "
                    "  to_char(created_at AT TIME ZONE 'UTC', "
                    "          'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"')",
                    { std::to_string(product_id), type_str, qty_str, target });
                if (!ins.is_ok()) throw std::runtime_error("insert failed");

                response = { 201, "application/json; charset=utf-8",
                             operation_to_json(ins.get()).dump() };
            }
        });
    } catch (const std::exception& e) {
        if (std::string(e.what()) == "INSUFFICIENT_STOCK")
            return from_error(ApiError::insufficient_stock(
                "Недостаточно товара на складе для списания"));
        return from_error(ApiError::internal(e.what()));
    }

    if (!committed)
        return from_error(ApiError::internal("transaction rolled back"));

    return response;
}

// GET /api/operations
HttpResponse handle_list_operations(DbPool& db) {
    const char* sql =
        "SELECT o.id, o.product_id, p.name, o.type, o.quantity, "
        "       o.result_status, "
        "       to_char(o.created_at AT TIME ZONE 'UTC', "
        "               'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"') "
        "FROM operations o "
        "JOIN products p ON o.product_id = p.id "
        "ORDER BY o.created_at DESC, o.id DESC";

    auto res = db.execParams(sql, {});
    if (!res.is_ok())
        return from_error(ApiError::internal("DB query error"));

    json arr = json::array();
    for (int i = 0; i < PQntuples(res.get()); ++i)
        arr.push_back(operation_to_json(res.get(), i));

    return { 200, "application/json; charset=utf-8", arr.dump() };
}