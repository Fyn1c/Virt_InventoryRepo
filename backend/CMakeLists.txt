cmake_minimum_required(VERSION 3.23)
project(backend-service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(MyFirstApp
    main.cpp
    src/routes/operations_routes.cpp
)


add_executable(seed_db
    src/routes/seed.cpp
)