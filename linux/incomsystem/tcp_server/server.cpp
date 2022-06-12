#include "server.h"

Server::Server(int port) : _port(port)
{

}

int Server::exec()
{
    // создание сокета ip4 TCP
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    Logger::write("Starting echo server on the port " + std::to_string(_port) + "...");

    if (_socket == -1)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    // установка опций для сокета - привязку к порту даже если он еще находится в состоянии TIME_WAIT
    int enable = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    // структура адреса для сервера
    sockaddr_in addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(_port),
    };
    addr.sin_addr.s_addr = INADDR_ANY;

    int addrlen = sizeof(addr);

    // связывание сокета и адреса
    if (bind(_socket, (const sockaddr*)&addr, sizeof(addr)) != 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    // слушать соединения на сокете
    if (listen(_socket, 0) < 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }
    _state = State::Listening;

    Logger::write("Running echo server...");

    // необходимо для функции select
    timeval tv
    {
        tv.tv_sec = SOCKET_TIMEOUT,
        tv.tv_usec = 0
    };
    fd_set readfds;

    // запуск основного цикла
    while(state() == State::Listening)
    {
        // необходимо для функции select
        FD_ZERO(&readfds);
        FD_SET(_socket, &readfds);

        // таймаут для accept
        if (select(_socket + 1, &readfds, NULL, NULL, &tv) == -1)
        {
            LOG_ERROR_STRING;
            break;
        }

        if (FD_ISSET(_socket, &readfds))
        {
            // ожидание подключения новых клиентов
            int client_sock;
            if ( (client_sock = accept(_socket, (sockaddr *)&addr, (socklen_t*)&addrlen)) < 0)
            {
                LOG_ERROR_STRING;
                continue;
            }

            // поток для нового подключенного клиента
            std::thread th(&Server::clientThread, this, client_sock);
            th.detach();
        }
        // задержка используется для разгрузки процессора
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    _state = State::Unconnected;

    return EXIT_SUCCESS;
}

void Server::clientThread(int socket) const
{
    // сохраняем информацию о сокете
    auto hInfo = hostInfo(socket);

    Logger::write(hInfo + " - connected");

    // объект клиент, состояние - подключен, так как сокет уже был создан
    Client client(socket);
    // запуск основного цикла
    client.exec();

    Logger::write(hInfo + " - disconnected");
}
