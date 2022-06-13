#include "simplyclient.h"

SimplyClient::SimplyClient(const std::string &host, uint64_t port) : _host(host),
    _port(port)
{

}

int SimplyClient::exec()
{
    // создание сокета ip4 TCP
    if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    // получение указателя на структуру hostent из переменной host
    const hostent *remote_host { gethostbyname(_host.c_str()) };
    if (remote_host == nullptr)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    // структура адреса для сервера
    sockaddr_in server_addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(_port)
    };
    server_addr.sin_addr.s_addr = *(const in_addr_t*)remote_host->h_addr;

    // соединение с сервером
    if (connect(_socket, (const sockaddr* const)&server_addr, sizeof(server_addr)) != 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    _state = State::Connected;
    _session++;

    // установка опций для сокета - привязку к порту даже если он еще находится в состоянии TIME_WAIT
    int enable = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }
    Logger::write("Connected to " + Socket::hostInfo(_socket));

    // поток для получения данных от сервера
    std::thread reciveThread(&SimplyClient::getAnswer, this);
    // поток для отправки данных от сервера
    std::thread sendThread(&SimplyClient::sendRequest, this);

    // ожидание завершения потоков
    reciveThread.join();
    sendThread.join();

    return EXIT_SUCCESS;
}

void SimplyClient::getAnswer()
{
    char buffer[BUFSIZ];

    while (state() == State::Connected)
    {
        // ожидание получения данных с сервера
        auto recv_bytes = recv(_socket, (void *)buffer, BUFSIZ - 1, 0);
        if (recv_bytes <= 0)
        {
            Logger::write("Disconnected from "
                          + Socket::hostInfo(_socket)
                          + "\nPress any key");
            break;
        }

        buffer[recv_bytes] = '\0';

        // вывод полученных данных в консоль и запись в лог файл
        Logger::write(Socket::hostInfo(_socket)
                      + " - << "
                      + buffer);

        // задержка используется для разгрузки процессора
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    _state = State::Unconnected;
}

void SimplyClient::sendRequest()
{
    std::string buffer;

    while (state() == State::Connected)
    {
        // ввод данных для отправки
        std::getline(std::cin, buffer);

        if (buffer.length() == 0)
            continue;

        // отправка на сервер
        if (send(_socket, (const void *)buffer.data(), buffer.length(), 0) <= 0)
            break;

        // вывод полученных данных в консоль и запись в лог файл
        Logger::write(Socket::hostInfo(_socket)
                      + " - >> "
                      + buffer);
    }
    _state = State::Unconnected;
}
