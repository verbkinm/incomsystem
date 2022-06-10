#include "server.h"

Server::Server(int port) : _port(port)
{

}

int Server::exec()
{
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Logger::write("Starting echo server on the port " + std::to_string(_port) + "...");

    if (_socket == -1)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    int enable = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    sockaddr_in addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(_port),
    };
    addr.sin_addr.s_addr = INADDR_ANY;

    int addrlen = sizeof(addr);

    if (bind(_socket, (const sockaddr*)&addr, sizeof(addr) != 0))
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }

    if (listen(_socket, 0) < 0)
    {
        LOG_ERROR_STRING;
        return EXIT_FAILURE;
    }
    setState(State::Listening);

    Logger::write("Running echo server...");

    timeval tv
    {
        tv.tv_sec = SOCKET_TIMEOUT,
        tv.tv_usec = 0
    };
    fd_set readfds;

    while(state() == State::Listening)
    {
        FD_ZERO(&readfds);
        FD_SET(_socket, &readfds);

        if (select(_socket + 1, &readfds, NULL, NULL, &tv) == -1)
        {
            LOG_ERROR_STRING;
            break;
        }

        if (FD_ISSET(_socket, &readfds))
        {
            int client_sock;
            if ( (client_sock = accept(_socket, (sockaddr *)&addr, (socklen_t*)&addrlen)) < 0)
            {
                LOG_ERROR_STRING;
                setState(State::Closing);
                continue;
            }

            std::thread th(&Server::clientThread, this, client_sock);
            th.detach();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    setState(State::Unconnected);

    return EXIT_SUCCESS;
}

void Server::clientThread(int socket) const
{
    auto hInfo = hostInfo(socket);

    Logger::write(hInfo + " - connected");

    Client client(socket);
    client.setState(Socket::State::Connected);
    client.exec();

    Logger::write(hInfo + " - disconnected");
}
