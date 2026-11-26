#include <boost/beast/core.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "../include/server.h"
#include "../include/operations_routes.hpp"
#include "../include/product_routes.h"
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

namespace beast = boost::beast;
namespace http  = boost::beast::http;
namespace net   = boost::asio;
namespace sys   = boost::system;
using tcp = boost::asio::ip::tcp;
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
    sys::error_code ec;
    beast::flat_buffer buffer;

    http::request<http::string_body> request;
    http::read(socket, buffer, request, ec);

    if (ec == http::error::end_of_stream) return;
    if (ec) { std::cerr << "Read error: " << ec.message() << "\n"; return; }

    HttpResponse our_response{404, "application/json; charset=utf-8",
                              R"({"code":"not_found","message":"Not Found"})"};
    std::string target = std::string(request.target());
    std::string method = std::string(request.method_string());

    // путь без query-строки
    std::string path = target;
    std::string query;
    auto qpos = target.find('?');
    if (qpos != std::string::npos) {
        path  = target.substr(0, qpos);
        query = target.substr(qpos + 1);
    }

    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string item;
    while (std::getline(ss, item, '/')) if (!item.empty()) parts.push_back(item);

    // GET /statuses
    if (method == "GET" && path == "/statuses") {
        our_response = handle_get_statuses();
    }
    // GET /operations
    else if (method == "GET" && path == "/operations") {
        our_response = handle_list_operations(db_);
    }
    // GET /products
    else if (method == "GET" && path == "/products") {
        std::string search;
        auto spos = query.find("search=");
        if (spos != std::string::npos) {
            search = query.substr(spos + 7);
            auto amp = search.find('&');
            if (amp != std::string::npos) search = search.substr(0, amp);
            std::replace(search.begin(), search.end(), '+', ' ');
        }
        our_response = handle_list_products(db_, search);
    }
    // POST /products
    else if (method == "POST" && path == "/products") {
        our_response = handle_create_product(db_, std::string(request.body()));
    }
    // GET /products/{id}
    else if (method == "GET" && parts.size() == 2 && parts[0] == "products") {
        our_response = handle_get_product(db_, std::stoi(parts[1]));
    }
    // DELETE /products/{id}
    else if (method == "DELETE" && parts.size() == 2 && parts[0] == "products") {
        our_response = handle_delete_product(db_, std::stoi(parts[1]));
    }
    // POST /products/{id}/operations
    else if (method == "POST" && parts.size() == 3 &&
             parts[0] == "products" && parts[2] == "operations") {
        our_response = handle_create_operation(db_, std::stoi(parts[1]),
                                               std::string(request.body()));
    }

    http::response<http::string_body> result{
        static_cast<http::status>(our_response.status_code), request.version()
    };
    result.set(http::field::server, "Warehouse-Backend");
    result.set(http::field::content_type, our_response.content_type);
    result.body() = our_response.body;
    result.prepare_payload();

    http::write(socket, result, ec);
    if (ec) { std::cerr << "Write error: " << ec.message() << "\n"; return; }

    socket.shutdown(tcp::socket::shutdown_send, ec);
}