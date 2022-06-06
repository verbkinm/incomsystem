#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string.h>

#define LOG_ERROR_STRING Logger::error(std::string(__PRETTY_FUNCTION__) + ", line:" + std::to_string(__LINE__) + " - error message: ");
#define LOG_FILE_NAME "tcp_server.log"

class Logger
{
private:
    static std::ofstream _file;
    static std::mutex _file_mutex;

    Logger();
    ~Logger();
    Logger(const Logger &) = delete;
    Logger& operator=(Logger &) = delete;

public:
    static Logger &getInstance();

    static std::string currentDateTime();


    static void write(const std::string &str);
    static void error(const std::string &str);
};
