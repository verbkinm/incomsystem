#include <signal.h>

#include "proxyserver.h"

// обработчик сигналов
void signalHandler(int signal);
// функция для проверки аргументов командной строки
bool checkArgs(int argc, const char * const argv[], std::string &serverHost, uint64_t &serverPort, uint64_t &listenPort);

int main(int argc, char const *argv[])
{
    // объявляем функцию обработчика сигнала SIGINT
    signal(SIGINT, signalHandler);

    // инициализируем Logger
    Logger::init();

    std::string serverHost;
    uint64_t serverPort, listenPort;

    // проверка аргументов командной строки
    if (!checkArgs(argc, argv, serverHost, serverPort, listenPort))
        return EXIT_FAILURE;

    // создание объекта класса ProxyServer и его запуск
    ProxyServer proxyServer(serverHost, serverPort, listenPort);

    return proxyServer.exec();
}

void signalHandler(int signal)
{
    // необходимо для корректного вызова деструктора для объекта proxyServer в main
    if (signal == SIGINT)
        exit(0);
}

bool checkArgs(int argc, const char * const argv[], std::string &serverHost, uint64_t &serverPort, uint64_t &listenPort)
{
    std::string msg = "Usage: " + std::string(argv[0]) + " <server host> <server port> <listen port>";
    // если количество введённых аргументов не соответствует 3 выводим сообщение и завершает программу
    if (argc != 4)
    {
        Logger::write(msg);
        return false;
    }

    try
    {
        serverHost = argv[1];
        // stoull может выкинуть исключение, по этому оборачиваем этот код в блок try catch
        serverPort = std::stoull(argv[2]);
        listenPort = std::stoull(argv[3]);
    }
    catch (...)
    {
        Logger::write(msg);
        return false;
    }

    return true;
}
