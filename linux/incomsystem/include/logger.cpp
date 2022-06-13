#include "logger.h"

// создание объекта file для работы с файлом и открытие файла в режиме дозаписи
std::ofstream Logger::_file(LOG_FILE_NAME, std::ios::app);
std::mutex Logger::_write_mutex;
// флаг для режима записи
uint8_t Logger::_flags(0);

Logger::Logger(int flags)
{
    _flags = flags;
    auto str = "Start program\n-----------------------------------\n";
    write(str);
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
    auto str = "Exit program\n-----------------------------------\n";

    write(str);
    _file.close();
}

void Logger::init(int flags)
{
    static Logger instance(flags);
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

    if (_flags & CONSOLE)
        std::cout << result;
    if (_flags & FILE)
        _file << result;
}

void Logger::error(const std::string &str)
{
    // буфер для текстовой интерпретации ошибки
    char buffer[BUFSIZ];
    Logger::write(str + ' ' + strerror_r(errno, &buffer[0], BUFSIZ));
}
