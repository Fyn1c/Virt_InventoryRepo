#pragma once
#include <string>
#include <optional>
#include "data_base_pool.h"

struct HttpResponse {
    int status_code = 200;
    std::string content_type = "application/json; charset=utf-8";
    std::string body;
};

HttpResponse handle_create_operation(DbPool& db, int product_id, const std::string& raw_body);
HttpResponse handle_list_operations(DbPool& db);

std::optional<int> extract_product_id(const std::string& target);