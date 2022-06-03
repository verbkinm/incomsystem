#include "client.h"
#include <fcntl.h>

Client::Client(int &socket) : _socket(socket)
{

}

Client::~Client()
{
    close(_socket);
    _socket = -1;
}

void Client::exec() const
{
    while (true)
    {
        char buff[BUFSIZ];

        auto recv_bytes = recv(_socket, (void *)buff, BUFSIZ - 1, 0);
        if (recv_bytes <= 0)
            break;

        buff[recv_bytes] = '\0';

        std::cout << general::currentDateTime() << ' ' << general::hostInfo(_socket)
                  << " - " << buff << "\n";

        if (send(_socket, (const void *)buff, recv_bytes, 0) <= 0)
            exit(EXIT_FAILURE);
    }
}

int Client::socket() const
{
    return _socket;
}

