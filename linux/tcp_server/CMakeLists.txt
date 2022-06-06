cmake_minimum_required(VERSION 3.5)

project(tcp_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tcp_server main.cpp client.h client.cpp lib.h lib.cpp server.h server.cpp socket.h socket.cpp)
