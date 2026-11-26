#pragma once
#include <cstdlib>
#include <string>
#include <stdexcept>

struct Config {
    std::string host = "0.0.0.0";
    unsigned short port = 8080;
    
    std::string db_host = "localhost";
    std::string db_port = "5432";
    std::string db_name = "postgres";
    std::string db_user = "postgres";
    std::string db_password = "admin";

    static Config from_env() {
        Config cfg;
        if (const char* v = std::getenv("PORT")) cfg.port = static_cast<unsigned short>(std::stoi(v));
        if (const char* v = std::getenv("DB_HOST")) cfg.db_host = v;
        if (const char* v = std::getenv("DB_PORT")) cfg.db_port = v;
        if (const char* v = std::getenv("DB_NAME")) cfg.db_name = v;
        if (const char* v = std::getenv("DB_USER")) cfg.db_user = v;
        if (const char* v = std::getenv("DB_PASSWORD")) cfg.db_password = v;
        return cfg;
    }

    std::string db_connection_string() const {
        return "host=" + db_host + " port=" + db_port + " dbname=" + db_name +
               " user=" + db_user + " password=" + db_password;
    }
};