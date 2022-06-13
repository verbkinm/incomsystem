#include "baseserver.h"

BaseServer::BaseServer()
{

}

int BaseServer::listenSocketCreate()
{
    // структура адреса для сервера
    sockaddr_in addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(_listenPort),
    };
    addr.sin_addr.s_addr = INADDR_ANY;

    // создание сокета ip4 TCP
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        LOG_ERROR_STRING;
        return -1;
    }

    // установка опций для сокета - привязку к порту даже если он еще находится в состоянии TIME_WAIT
    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        LOG_ERROR_STRING;
        close(sock);
        return -1;
    }

    // связывание сокета и адреса
    if (bind(sock, (const sockaddr*)&addr, sizeof(addr)) != 0)
    {
        LOG_ERROR_STRING;
        close(sock);
        return -1;
    }

    // слушать соединения на сокете
    if (listen(sock, 0) < 0)
    {
        LOG_ERROR_STRING;
        close(sock);
        return -1;
    }
    _state = State::Listening;

    return sock;
}
