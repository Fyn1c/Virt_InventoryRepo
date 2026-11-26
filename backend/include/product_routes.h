#pragma once
#include "operations_routes.hpp"
#include "data_base_pool.h"
#include <string>

HttpResponse handle_list_products(DbPool& db, const std::string& search);
HttpResponse handle_get_product(DbPool& db, int product_id);
HttpResponse handle_create_product(DbPool& db, const std::string& raw_body);
HttpResponse handle_delete_product(DbPool& db, int product_id);
HttpResponse handle_get_statuses();