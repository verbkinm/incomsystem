cmake_minimum_required(VERSION 3.5)

project(tcp_proxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tcp_proxy main.cpp
    client.h client.cpp
    socket.h socket.cpp
    proxyserver.h proxyserver.cpp
    logger.h logger.cpp)
