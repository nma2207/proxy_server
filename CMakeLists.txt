cmake_minimum_required(VERSION 3.5)

project(proxy_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include/)
add_executable(proxy_server src/main.cpp
    src/server.cpp
    src/postgresql_manager.cpp
    src/message_parser.cpp)
