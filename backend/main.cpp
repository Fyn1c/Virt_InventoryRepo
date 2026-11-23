#include <iostream>
#include "./src/routes/operations_routes.hpp"

int main() {
    const auto created = handle_create_operation_mock(1, R"({"type":"income","quantity":10})");
    std::cout << "POST mock status: " << created.status_code << "\n" << created.body << "\n\n";


    const auto journal = handle_list_operations_mock();
    std::cout << "GET mock status: " << journal.status_code << "\n" << journal.body << "\n";

    return 0;
}