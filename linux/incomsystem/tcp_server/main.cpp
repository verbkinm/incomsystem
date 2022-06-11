#include <signal.h>

#include "server.h"

// обработчик сигналов
void signalHandler(int signal);
// функция для проверки аргументов командной строки
bool checkArgs(int argc, const char * const argv[], uint64_t &listenPort);

int main(int argc, char const *argv[])
{
    // объявляем функцию обработчика сигнала SIGINT
    signal(SIGINT, signalHandler);

    // инициализируем Logger
    Logger::init();

    uint64_t listenPort;

    // проверка аргументов командной строки
    if (!checkArgs(argc, argv, listenPort))
        return EXIT_FAILURE;

    // создание объекта класса Server и его запуск
    Server server(listenPort);
    server.exec();
}

void signalHandler(int signal)
{
    // необходимо для корректного вызова деструктора для объекта server в main
    if (signal == SIGINT)
        exit(0);
}

bool checkArgs(int argc, const char * const argv[], uint64_t &listenPort)
{
    // если количество введённых аргументов не соответствует 2 выводим сообщение и завершает программу
    std::string msg = "Usage: " + std::string(argv[0]) + " <listen port>";
    if (argc != 2)
    {
        Logger::write(msg);
        return false;
    }

    try
    {
        // stoull может выкинуть исключение, по этому оборачиваем этот код в блок try catch
        listenPort = std::stoull(argv[1]);
    }
    catch (...)
    {
        Logger::write(msg);
        return false;
    }

    return true;
}
