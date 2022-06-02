cmake_minimum_required(VERSION 3.5)

project(tcp_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tcp_server main.cpp client.h client.cpp)
