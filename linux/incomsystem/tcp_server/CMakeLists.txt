cmake_minimum_required(VERSION 3.5)

project(tcp_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("../include")

add_executable(tcp_server main.cpp
    client.h client.cpp
    server.h server.cpp
    ../include/socket.h ../include/socket.cpp
    ../include/logger.h ../include/logger.cpp
    ../include/baseserver.h ../include/baseserver.cpp)
