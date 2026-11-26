cmake_minimum_required(VERSION 3.23)
project(backend-service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS beast asio)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(libpqxx Config REQUIRED)

add_executable(MyFirstApp
    main.cpp
    src/data_base_pool.cpp
    src/errors.cpp
    src/product_routes.cpp
    src/operations_routes.cpp
    src/server.cpp
)

target_include_directories(MyFirstApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $(CMAKE_CURRENT_SOURCE_DIR)/include
)

target_link_libraries(MyFirstApp PRIVATE
    Boost::beast
    Boost::asio
    nlohmann_json::nlohmann_json
    PostgreSQL::PostgreSQL
    libpqxx::pqxx
)
