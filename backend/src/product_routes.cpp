#include "../include/product_routes.h"
#include "../include/errors.h"
#include "../include/validation.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
HttpResponse from_error(const ApiError& e) {
    return { e.http_status, "application/json; charset=utf-8", e.to_json().dump() };
}
}  // namespace

// GET /api/products?search=
HttpResponse handle_list_products(DbPool& db, const std::string& search) {
    std::string sql =
        "SELECT p.id, p.name, p.status, "
        "       to_char(p.created_at AT TIME ZONE 'UTC', "
        "               'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"'), "
        "       COALESCE(SUM(CASE WHEN o.type='приход' THEN o.quantity "
        "                         WHEN o.type='расход' THEN -o.quantity "
        "                         ELSE 0 END), 0) "
        "FROM products p "
        "LEFT JOIN operations o ON o.product_id = p.id ";

    std::vector<std::string> params;
    if (!search.empty()) {
        sql += "WHERE p.name ILIKE '%' || $1 || '%' ";
        params.push_back(search);
    }
    sql += "GROUP BY p.id ORDER BY p.id";

    auto res = db.execParams(sql, params);
    if (!res.is_ok())
        return from_error(ApiError::internal("DB error"));

    json arr = json::array();
    for (int i = 0; i < PQntuples(res.get()); ++i) {
        arr.push_back({
            {"id",        std::stoi(PQgetvalue(res.get(), i, 0))},
            {"name",      PQgetvalue(res.get(), i, 1)},
            {"status",    PQgetvalue(res.get(), i, 2)},
            {"createdAt", PQgetvalue(res.get(), i, 3)},
            {"stock",     std::stoll(PQgetvalue(res.get(), i, 4))}
        });
    }
    return { 200, "application/json; charset=utf-8", arr.dump() };
}

// GET /api/products/{id}
HttpResponse handle_get_product(DbPool& db, int product_id) {
    std::string sql =
        "SELECT p.id, p.name, p.status, "
        "       to_char(p.created_at AT TIME ZONE 'UTC', "
        "               'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"'), "
        "       COALESCE(SUM(CASE WHEN o.type='приход' THEN o.quantity "
        "                         WHEN o.type='расход' THEN -o.quantity "
        "                         ELSE 0 END), 0) "
        "FROM products p "
        "LEFT JOIN operations o ON o.product_id = p.id "
        "WHERE p.id = $1 GROUP BY p.id";

    auto res = db.execParams(sql, { std::to_string(product_id) });
    if (!res.is_ok())
        return from_error(ApiError::internal("DB error"));
    if (PQntuples(res.get()) == 0)
        return from_error(ApiError::not_found("Товар не найден"));

    json product = {
        {"id",        std::stoi(PQgetvalue(res.get(), 0, 0))},
        {"name",      PQgetvalue(res.get(), 0, 1)},
        {"status",    PQgetvalue(res.get(), 0, 2)},
        {"createdAt", PQgetvalue(res.get(), 0, 3)},
        {"stock",     std::stoll(PQgetvalue(res.get(), 0, 4))}
    };

    std::string opsSql =
        "SELECT id, product_id, "
        "       (SELECT name FROM products WHERE id = $1), "
        "       type, quantity, result_status, "
        "       to_char(created_at AT TIME ZONE 'UTC', "
        "               'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"') "
        "FROM operations WHERE product_id = $1 "
        "ORDER BY created_at DESC, id DESC";

    auto ops = db.execParams(opsSql, { std::to_string(product_id) });
    if (!ops.is_ok())
        return from_error(ApiError::internal("DB error"));

    product["operations"] = json::array();
    for (int i = 0; i < PQntuples(ops.get()); ++i) {
        json item;
        item["id"]          = std::stoi(PQgetvalue(ops.get(), i, 0));
        item["productId"]   = std::stoi(PQgetvalue(ops.get(), i, 1));
        item["productName"] = PQgetvalue(ops.get(), i, 2);
        item["type"]        = PQgetvalue(ops.get(), i, 3);
        item["quantity"]    = std::stoi(PQgetvalue(ops.get(), i, 4));
        if (PQgetisnull(ops.get(), i, 5))
            item["resultStatus"] = nullptr;
        else
            item["resultStatus"] = PQgetvalue(ops.get(), i, 5);
        item["createdAt"]   = PQgetvalue(ops.get(), i, 6);
        product["operations"].push_back(item);
    }

    return { 200, "application/json; charset=utf-8", product.dump() };
}

// POST /api/products
HttpResponse handle_create_product(DbPool& db, const std::string& raw_body) {
    auto body = nlohmann::json::parse(raw_body, nullptr, false);
    if (body.is_discarded())
        return from_error(ApiError::validation_error({{"body", "invalid JSON"}}));

    auto v = validation::validateProductCreate(body);
    if (!v.ok())
        return from_error(ApiError::validation_error(v.errors));

    auto res = db.execParams(
        "INSERT INTO products (name) VALUES ($1) "
        "RETURNING id, name, status, "
        "  to_char(created_at AT TIME ZONE 'UTC', "
        "          'YYYY-MM-DD\"T\"HH24:MI:SS\"Z\"')",
        { body["name"].get<std::string>() });
    if (!res.is_ok())
        return from_error(ApiError::internal("DB insert error"));

    json product = {
        {"id",        std::stoi(PQgetvalue(res.get(), 0, 0))},
        {"name",      PQgetvalue(res.get(), 0, 1)},
        {"status",    PQgetvalue(res.get(), 0, 2)},
        {"createdAt", PQgetvalue(res.get(), 0, 3)},
        {"stock",     0}
    };
    return { 201, "application/json; charset=utf-8", product.dump() };
}

// DELETE /api/products/{id}
HttpResponse handle_delete_product(DbPool& db, int product_id) {
    auto res = db.execParams("DELETE FROM products WHERE id = $1",
                             { std::to_string(product_id) });
    if (!res.is_command_ok())
        return from_error(ApiError::internal("DB error"));

    std::string affected = PQcmdTuples(res.get());
    if (affected == "0" || affected.empty())
        return from_error(ApiError::not_found("Товар не найден"));

    return { 204, "application/json; charset=utf-8", "" };
}

// GET /api/statuses
HttpResponse handle_get_statuses() {
    json arr = json::array();
    for (const auto& s : validation::storageStatuses())
        arr.push_back(s);
    return { 200, "application/json; charset=utf-8",
             json{{"statuses", arr}}.dump() };
}