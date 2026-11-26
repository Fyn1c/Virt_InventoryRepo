#pragma once
#include "operations_routes.hpp"
#include "data_base_pool.h"


HttpResponse handle_list_products(DbPool& db);
HttpResponse handle_get_product(DbPool& db, int product_id);
HttpResponse handle_create_product(DbPool& db, const std::string& raw_body);
HttpResponse handle_update_product_status(DbPool& db, int product_id, const std::string& raw_body);