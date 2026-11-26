cmake_minimum_required(VERSION 3.23)
project(backend-service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(PostgreSQL REQUIRED)

add_executable(backend-service
    main.cpp
    src/data_base_pool.cpp
    src/errors.cpp
    src/validation.cpp
    src/product_routes.cpp
    src/operations_routes.cpp
    src/server.cpp
)

target_include_directories(backend-service PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(backend-service PRIVATE
    Boost::boost
    nlohmann_json::nlohmann_json
    PostgreSQL::PostgreSQL
)