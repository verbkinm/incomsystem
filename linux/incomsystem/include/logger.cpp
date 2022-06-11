#include "logger.h"

// создание объекта file для работы с файлом и открытие файла в режиме дозаписи
std::ofstream Logger::_file(LOG_FILE_NAME, std::ios::app);
std::mutex Logger::_write_mutex;

Logger::Logger()
{
    auto str = "-----------------------------------\n"
            + currentDateTime()
            + " - Start program\n";

    _file << str;
    std::cout << str;
}

std::string Logger::currentDateTime()
{
    time_t now = time(0);
    tm tstruct;
    char buf[32];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

Logger::~Logger()
{
    auto str = currentDateTime()
            + " - Exit program\n-----------------------------------\n";

    _file << str;
    _file.close();
    std::cout << str;
}

void Logger::init()
{
    static Logger instance;
}

void Logger::write(const std::string &str)
{
    std::string result = currentDateTime() + " - " + str + '\n';

    // блокировка мьютекса на время записи в файл и вывода в консоль.
    std::lock_guard lg(_write_mutex);

    // проверка открытия файла
    if (!_file.is_open())
    {
        std::cerr << currentDateTime()
                  << " - Can't open log file "
                  << "\"" << LOG_FILE_NAME << "\"\n";
        return;
    }

    _file << result;
    std::cout << result;
}

void Logger::error(const std::string &str)
{
    // буфер для текстовой интерпритации ошибки
    char buffer[BUFSIZ];
    Logger::write(str + ' ' + strerror_r(errno, &buffer[0], BUFSIZ));
}
