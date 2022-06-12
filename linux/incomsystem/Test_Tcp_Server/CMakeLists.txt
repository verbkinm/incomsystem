cmake_minimum_required(VERSION 3.5)

project(Tests LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories("../tcp_server")
include_directories("../tcp_client")
include_directories("../include")

add_executable(Tests tst_tests.cpp
    ../tcp_client/simplyclient.h ../tcp_client/simplyclient.cpp
    ../tcp_server/client.h ../tcp_server/client.cpp
    ../tcp_server/server.h ../tcp_server/server.cpp
    ../include/socket.h ../include/socket.cpp
    ../include/logger.h ../include/logger.cpp)
add_test(NAME Tests COMMAND Tests)

target_link_libraries(Tests PRIVATE Qt${QT_VERSION_MAJOR}::Test)

