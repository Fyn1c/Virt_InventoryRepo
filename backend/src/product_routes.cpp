#include "../include/product_routes.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>


namespace {
    std::string get_time_now() {
        using namespace std::chrono;
        const auto now = system_clock::now();
        const std::time_t t = system_clock::to_time_t(now);
        std::tm tm_utc{};
        #if defined(_WIN32)
        gmtime_s(&tm_utc, &t);
        #else
        gmtime_r(&t, &tm_utc);
        #endif
        std::ostringstream oss;
        oss << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%SZ");
        return oss.str();
    }
}
using json = nlohmann::json;

HttpResponse handle_list_products(DbPool& data_base) {
    std::string sql = "SELECT id, name, status, created_at FROM products ORDER BY id";
    auto res = data_base.execParams(sql, {});
    if (!res.is_ok()){
        return {500, "application/json", R"({"error": "DB error"})"};
    }

    json response;
    response["items"] = json::array();
    int rows = PQntuples(res.get());
    for (int i = 0; i < rows; ++i) {
        response["items"].push_back({
            {"id", std::stoi(PQgetvalue(res.get(), i, 0))},
            {"name", PQgetvalue(res.get(), i, 1)},
            {"status", PQgetvalue(res.get(), i, 2)},
            {"created_at", PQgetvalue(res.get(), i, 3)}
        });
    }
    return {200, "application/json", response.dump()};
}

HttpResponse handle_get_product(DbPool& data_base, int product_id) {
    std::string sql = "SELECT id, name, status, created_at FROM products WHERE id = $1";
    auto res = data_base.execParams(sql, {std::to_string(product_id)});
    if (!res.is_ok()){
        return {500, "application/json", R"({"error": "DB error"})"};
    }
    if (PQntuples(res.get()) == 0){
        return {404, "application/json", R"({"error": "Not found"})"};
    }

    json response = {
        {"id", std::stoi(PQgetvalue(res.get(), 0, 0))},
        {"name", PQgetvalue(res.get(), 0, 1)},
        {"status", PQgetvalue(res.get(), 0, 2)},
        {"created_at", PQgetvalue(res.get(), 0, 3)}
    };
    return {200, "application/json", response.dump()};
}

HttpResponse handle_create_product(DbPool& data_base, const std::string& raw_body) {
    auto body = nlohmann::json::parse(raw_body, nullptr, false);
    if (body.is_discarded())
        return from_error(ApiError::validation_error({{"body", "invalid JSON"}}));

    auto v = validation::validateProductCreate(body);
    if (!v.ok())
        return from_error(ApiError::validation_error(v.errors));

    auto res = db.execParams(
        "INSERT INTO products (name) VALUES ($1) RETURNING id, name, status, created_at",
        { body["name"].get<std::string>() });
    if (!res.is_ok())
        return from_error(ApiError::internal("DB insert error"));
    try {
        auto j = json::parse(raw_body);
        std::string name = j.value("name", "");
        if (name.empty()){
            return {400, "application/json", R"({"error": "Name required"})"};
        }

        std::string sql = "INSERT INTO products (name) VALUES ($1) RETURNING id, status, created_at";
        auto res = data_base.execParams(sql, {name});
        if (!res.is_ok()){
            return {400, "application/json", R"({"error": "DB insert error"})"};
        }

        json response = {
            {"id", std::stoi(PQgetvalue(res.get(), 0, 0))},
            {"name", name},
            {"status", PQgetvalue(res.get(), 0, 1)},
            {"created_at", PQgetvalue(res.get(), 0, 2)}
        };
        return {201, "application/json", response.dump()};
    } catch (const std::exception& e) {
        return {400, "application/json", R"({"error": "Invalid JSON"})"};
    }
}

HttpResponse handle_update_product_status(DbPool& data_base, int product_id, const std::string& raw_body) {
    try {
        auto j = json::parse(raw_body);
        std::string new_status = j.value("status", "");
        if (new_status.empty()){
            return {400, "application/json", R"({"error": "Status required"})"};
        }

        std::string sql = "UPDATE products SET status = $1 WHERE id = $2 RETURNING id, status";
        auto res = data_base.execParams(sql, {new_status, std::to_string(product_id)});
        if (!res.is_ok()){
            return {400, "application/json", R"({"error": "DB update error"})"};
        }
        if (PQntuples(res.get()) == 0){
            return {404, "application/json", R"({"error": "Not found"})"};
        }

        json response = {
            {"id", std::stoi(PQgetvalue(res.get(), 0, 0))},
            {"status", PQgetvalue(res.get(), 0, 1)}
        };
        return {200, "application/json", response.dump()};
    } catch (...) {
        return {400, "application/json", R"({"error": "Invalid JSON"})"};
    }
}