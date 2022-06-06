cmake_minimum_required(VERSION 3.5)

project(incomsystem LANGUAGES CXX)

add_subdirectory("tcp_client")
add_subdirectory("tcp_server")
add_subdirectory("tcp_proxy")
