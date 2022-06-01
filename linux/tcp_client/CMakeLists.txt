cmake_minimum_required(VERSION 3.5)

project(tcp_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tcp_client main.cpp)
