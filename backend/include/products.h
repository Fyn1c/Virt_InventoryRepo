#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <string>
#include "database.h"
// йоу прост заглушка и т.д.
struct Product {
    int64_t id{};
    std::string name;
};

class ProductsRepository {
public:
    explicit ProductsRepository(Database& db) : db_(db) {}

    std::vector<Product>    list(const std::optional<std::string>& search);
    std::optional<Product>  get(int64_t id);
    Product                 create(const std::string& name);
    bool                    remove(int64_t id);

private:
    Database& db_;
};