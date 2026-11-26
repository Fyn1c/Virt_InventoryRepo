#include "include/server.h"
#include "include/config.h"
#include "include/data_base_pool.h"
#include <iostream>

int main() {
    try {
        Config config = Config::from_env();
        DbPool data_base(config.db_connection_string(), 4);
        Server server(config.host, config.port, data_base);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}