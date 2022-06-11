#include <signal.h>
#include <thread>

#include "logger.h"
#include "socket.h"

// переменная для хранения сокета
static int SOCKET;

// функция для ожидания приёма данных
void getAnswer(int sock);
// функция для отправки данных
void sendRequest(int sock);
// функция для проверки аргументов командной строки
bool checkArgs(int argc, const char * const argv[], std::string &host, uint64_t &port);

// обработчик сигналов
void signalHandler(int signal);
// функция перед завершением программы
void atExitFunc();

int main(int argc, const char * const argv[])
{
    // объявляем функцию завершения программы и обработчик сигнала SIGINT
    atexit(atExitFunc);
    signal(SIGINT, signalHandler);

    // инициализируем Logger
    Logger::init();

    std::string host;
    uint64_t port;

    sockaddr_in server_addr;

    // проверка аргументов командной строки
    if (!checkArgs(argc, argv, host, port))
        return EXIT_FAILURE;

    // создание сокета ip4 TCP
    if ((SOCKET = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    // получение указателя на структуру hostent из переменной host
    const hostent *remote_host { gethostbyname(host.c_str()) };
    if (remote_host == nullptr)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    // структура адреса для сервера
    server_addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };
    server_addr.sin_addr.s_addr = *(const in_addr_t*)remote_host->h_addr;

    // соединение с сервером
    if (connect(SOCKET, (const sockaddr* const)&server_addr, sizeof(server_addr)) != 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }
    // установка опций для сокета - привязку к порту даже если он еще находится в состоянии TIME_WAIT
    int enable = 1;
    if (setsockopt(SOCKET, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }
    Logger::write("Connected to " + Socket::hostInfo(SOCKET));

    // поток для получения данных от сервера
    std::thread reciveThread(getAnswer, SOCKET);
    // в этой функции вводятся данные для отправки на сервер
    sendRequest(SOCKET);

    // ожидание завершения потока reciveThread
    reciveThread.join();

    return EXIT_SUCCESS;
}

void getAnswer(int sock)
{
    char buffer[BUFSIZ];

    while (true)
    {
        // ожидание получения данных с сервера
        auto recv_bytes = recv(sock, (void *)buffer, BUFSIZ - 1, 0);
        if (recv_bytes <= 0)
        {
            Logger::write("Disconnected from " + Socket::hostInfo(sock));
            exit(EXIT_FAILURE);
        }

        buffer[recv_bytes] = '\0';

        // вывод полученных данных в консоль и запись в лог файл
        Logger::write(Socket::hostInfo(sock)
                      + " - << "
                      + buffer);

        // задержка используется для разгрузки процессора
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void sendRequest(int sock)
{
    std::string buffer;

    while (true)
    {
        // ввод данных для отправки
        std::getline(std::cin, buffer);

        if (buffer.length() == 0)
            continue;

        // отправка на сервер
        if (send(sock, (const void *)buffer.data(), buffer.length(), 0) <= 0)
            exit(EXIT_FAILURE);

        // вывод полученных данных в консоль и запись в лог файл
        Logger::write(Socket::hostInfo(sock)
                      + " - >> "
                      + buffer);
    }
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

void atExitFunc()
{
    shutdown(SOCKET, SHUT_RDWR);
}
