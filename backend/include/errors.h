#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <validation.h>

struct ApiError {
    int         http_status{};
    std::string code;
    std::string message;
    std::vector<FieldError> fields;

    nlohmann::json to_json() const;

    static ApiError validation(const std::vector<FieldError>& fields);
    static ApiError not_found(const std::string& what);
    static ApiError insufficient_stock(const std::string& message);
    static ApiError internal(const std::string& message);
    static ApiError not_implemented();
};