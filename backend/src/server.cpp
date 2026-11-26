#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "../include/server.h"
#include "../include/operations_routes.hpp"
#include "../include/product_routes.h"
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace net = boost::asio;
namespace tcp = net::ip::tcp;
namespace sys = boost::system;

Server::Server(const std::string& address, unsigned short port, DbPool& db)
    : address_(address), port_(port), db_(db) {}

void Server::run() {
    try {
        auto const addr = net::ip::make_address(address_);
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {addr, port_}};

        std::cout << "Server started on http://" << address_ << ":" << port_ << "\n";

        while (true) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            handle_session(socket); 
        }
    } catch (std::exception const& error) {
        std::cerr << "Server error: " << error.what() << "\n";
    }
}

void Server::handle_session(tcp::socket& socket) {
    sys::error_code error_code;
    beast::flat_buffer buffer;

    http::request<http::string_body> request;
    http::read(socket, buffer, request, error_code);

    if (error_code == http::error::end_of_stream) return;
    if (error_code) {
        std::cerr << "Read error: " << error_code.message() << "\n";
        return;
    }

    // --- РОУТЕР ---
    HttpResponse our_response{404, "application/json", R"({"error": "Not Found"})"};
    std::string target = std::string(request.target());
    std::string method = std::string(request.method_string());

    // Разбиваем URL на части: "/products/5/status" -> ["products", "5", "status"]
    std::vector<std::string> parts;
    std::stringstream ss(target);
    std::string item;
    while (std::getline(ss, item, '/')) {
        if (!item.empty()){
            parts.push_back(item);
        }
    }

    // 1. GET /operations
    if (method == "GET" && target == "/operations") {
        our_response = handle_list_operations(db_);
    }
    // 2. POST /operations или POST /products/{id}/operations
    else if (method == "POST" && !parts.empty() && parts.back() == "operations") {
        int product_id = (parts.size() == 2) ? 1 : std::stoi(parts[1]);
        our_response = handle_create_operation(db_, product_id, std::string(request.body()));
    }
    // 3. GET /products
    else if (method == "GET" && target == "/products") {
        our_response = handle_list_products(db_);
    }
    // 4. GET /products/{id}
    else if (method == "GET" && parts.size() == 2 && parts[0] == "products") {
        our_response = handle_get_product(db_, std::stoi(parts[1]));
    }
    // 5. POST /products
    else if (method == "POST" && target == "/products") {
        our_response = handle_create_product(db_, std::string(request.body()));
    }
    // 6. PATCH /products/{id}/status
    else if (method == "PATCH" && parts.size() == 3 && parts[0] == "products" && parts[2] == "status") {
        our_response = handle_update_product_status(db_, std::stoi(parts[1]), std::string(request.body()));
    }

    // --- ФОРМИРОВАНИЕ HTTP ОТВЕТА ---
    http::response<http::string_body> result{
        static_cast<http::status>(our_response.status_code), request.version()
    };
    result.set(http::field::server, "Warehouse-Backend");
    result.set(http::field::content_type, our_response.content_type);
    result.body() = our_response.body;
    result.prepare_payload();

    // --- ОТПРАВКА ---
    http::write(socket, result, error_code);
    if (error_code) {
        std::cerr << "Write error: " << error_code.message() << "\n";
        return;
    }

    socket.shutdown(tcp::socket::shutdown_send, error_code);
}