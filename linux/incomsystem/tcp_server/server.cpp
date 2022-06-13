#include "server.h"

Server::Server(int port) : BaseServer()
{
    _listenPort = port;
}

int Server::exec()
{
    // создание сокета ip4 TCP, на котором будет слушать сервер
    if ((_socket = listenSocketCreate()) == -1)
        return EXIT_FAILURE;

    Logger::write("Running echo server...");

    // необходимо для функции select
    timeval tv
    {
        tv.tv_sec = SOCKET_TIMEOUT,
        tv.tv_usec = 0
    };
    fd_set readfds;

    sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

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

void Server::clientThread(int socket)
{
    _session++;
    // сохраняем информацию о сокете
    auto hInfo = hostInfo(socket);

    Logger::write(hInfo + " - connected");

    // объект клиент, состояние - подключен, так как сокет уже был создан
    Client client(socket);
    // запуск основного цикла
    client.exec();

    Logger::write(hInfo + " - disconnected");
    _session--;
}
