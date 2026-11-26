#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include "data_base_pool.h"

class Server {
public:
    Server(const std::string& address, unsigned short port, DbPool& db);
    void run();

private:
    DbPool& db_;
    void handle_session(boost::asio::ip::tcp::socket& socket);

    std::string address_;
    unsigned short port_;
};