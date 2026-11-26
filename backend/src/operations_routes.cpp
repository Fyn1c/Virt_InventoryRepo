#include "../include/operations_routes.hpp"
#include "../include/product_routes.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <optional>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

HttpResponse handle_create_operation(DbPool& data_base, int product_id, const std::string& raw_body) {
    try {
        auto j = json::parse(raw_body);
        std::string type = j.value("type", "income");
        std::string status = j.value("status", "");
        std::string quantity = j.value("quantity", "0");

        if (type == "in" || type == "out") {
            std::string sql = 
                "INSERT INTO operations (product_id, type, quantity) "
                "VALUES ($1, $2, $3) RETURNING id, created_at";
            
            auto result = data_base.execParams(sql, {std::to_string(product_id), type, quantity});
            
            if (!result.is_ok()) {
                return {400, "application/json", R"({"error": "DB insert error"})"};
            }

            json response;
            response["id"] = std::stoi(PQgetvalue(result.get(), 0, 0));
            response["product_id"] = product_id;
            response["type"] = type;
            response["quantity"] = std::stoi(quantity);
            response["created_at"] = PQgetvalue(result.get(), 0, 1);
            return {201, "application/json", response.dump()};
        } 
        else if (type == "move") {
            std::string sql = 
                "INSERT INTO operations (product_id, type, from_status, to_status) "
                "VALUES ($1, $2, (SELECT status FROM products WHERE id = $1), $3) "
                "RETURNING id, created_at";
            
            auto result = data_base.execParams(sql, {std::to_string(product_id), type, status});
            
            if (!result.is_ok()) {
                return {400, "application/json", R"({"error": "DB insert error"})"};
            }

            json response;
            response["id"] = std::stoi(PQgetvalue(result.get(), 0, 0));
            response["product_id"] = product_id;
            response["type"] = type;
            response["new_status"] = status;
            response["created_at"] = PQgetvalue(result.get(), 0, 1);
            return {201, "application/json", response.dump()};
        }
        
        return {400, "application/json", R"({"error": "Unknown operation type"})"};

    } catch (const std::exception& e) {
        return {400, "application/json", std::string(R"({"error": ")") + e.what() + R"("})"};
    }
}

HttpResponse handle_list_operations(DbPool& db) {

    std::string sql = 
        "SELECT o.id, o.product_id, p.name, o.type, o.quantity, "
        "o.from_status, o.to_status, o.created_at "
        "FROM operations o "
        "JOIN products p ON o.product_id = p.id "
        "ORDER BY o.created_at DESC";

    auto result = db.execParams(sql, {});
    if (!result.is_ok()) {
        return {500, "application/json", R"({"error": "DB query error"})"};
    }

    json response;
    response["items"] = json::array();
    
    int rows = PQntuples(result.get());
    for (int i = 0; i < rows; ++i) {
        json item;
        item["id"] = std::stoi(PQgetvalue(result.get(), i, 0));
        item["product_id"] = std::stoi(PQgetvalue(result.get(), i, 1));
        item["product_name"] = PQgetvalue(result.get(), i, 2);
        item["type"] = PQgetvalue(result.get(), i, 3);
        
        if (!PQgetisnull(result.get(), i, 4)){
            item["quantity"] = std::stoi(PQgetvalue(result.get(), i, 4));
        }
        if (!PQgetisnull(result.get(), i, 5)){
            item["previous_status"] = PQgetvalue(result.get(), i, 5);
        }
        if (!PQgetisnull(result.get(), i, 6)){
            item["new_status"] = PQgetvalue(result.get(), i, 6);
        }
        
        item["created_at"] = PQgetvalue(result.get(), i, 7);
        response["items"].push_back(item);
    }
    return {200, "application/json", response.dump()};
}

