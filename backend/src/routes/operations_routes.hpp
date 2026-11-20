#pragma once

#include <string>

struct MockHttpResponse {
    int status_code = 200;
    std::string content_type = "application/json; charset=utf-8";
    std::string body;
};

MockHttpResponse handle_create_operation_mock(int product_id, const std::string& raw_body);
MockHttpResponse handle_list_operations_mock();