#include <signal.h>

#include "simplyclient.h"

// функция для проверки аргументов командной строки
bool checkArgs(int argc, const char * const argv[], std::string &host, uint64_t &port);

// обработчик сигналов
void signalHandler(int signal);

int main(int argc, const char * const argv[])
{
    // объявляем обработчик сигнала SIGINT
    signal(SIGINT, signalHandler);

    // инициализируем Logger
    Logger::init();

    std::string host;
    uint64_t port;

    // проверка аргументов командной строки
    if (!checkArgs(argc, argv, host, port))
        return EXIT_FAILURE;

    SimplyClient client(host, port);
    return client.exec();
}

bool checkArgs(int argc, const char * const argv[], std::string &host, uint64_t &port)
{
    std::string msg = "Usage: " + std::string(argv[0]) + " <host> <port>";
    // если количество введённых аргументов не соответствует 2 выводим сообщение и завершает программу
    if (argc != 3)
    {
        Logger::write(msg);
        return false;
    }

    try
    {
        host = argv[1];
        // stoull может выкинуть исключение, по этому оборачиваем этот код в блок try catch
        port = std::stoull(argv[2]);
    }
    catch (...)
    {
        Logger::write(msg);
        return false;
    }

    return true;
}

void signalHandler(int signal)
{
    // при получении сигнала
    if (signal == SIGINT)
        exit(0);
}
