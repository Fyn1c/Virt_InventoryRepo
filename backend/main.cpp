#include "include/server.h"
#include "include/config.h"
#include "include/data_base_pool.h"
#include <iostream>

int main() {
    Config config = Config::from_env();
    DbPool data_base(config.db_connection_string(), 4);
    Server server(config.host, config.port, data_base);
    server.run();
    return 0;
}