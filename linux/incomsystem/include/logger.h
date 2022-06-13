#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string.h>

// Во многих местах программы может быть ошибка, которую надо вывести вместе с местом получения
// этой ошибки. Следуя принципу DRY, используется define для вывода подробной информации
#define LOG_ERROR_STRING Logger::error(std::string(__PRETTY_FUNCTION__) + ", line:" + std::to_string(__LINE__) + " - error message: ")
// Имя лог файла
#define LOG_FILE_NAME "log.txt"

// Класс Logger создан на основе паттерна Singleton
class Logger
{
private:
    // объект для работы с лог-файлом
    static std::ofstream _file;

    // используется для конкурентной записи в файл
    static std::mutex _write_mutex;

    //флаг записи
    static uint8_t _flags;


    Logger(int flags);
    ~Logger();

    // удаление остальных конструкторов
    Logger() = delete;
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger& operator=(const Logger &) = delete;
    Logger& operator=(Logger &&) = delete;

public:
    enum writeType
    {
        NONE = 0,
        CONSOLE = 1,
        FILE = 2,
    };

    // метод инициализации объекта Logger
    static void init(int flags = CONSOLE | FILE);

    // строка с текущей датой и временем
    static std::string currentDateTime();

    // метод записи в лог файл и  вывода в консоль
    static void write(const std::string &str);

    //метод записи в лог файл и  вывода в консоль текстовой интерпритации ошибок errno
    static void error(const std::string &str);
};
