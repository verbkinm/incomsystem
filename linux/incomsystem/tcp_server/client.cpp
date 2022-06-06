#include "client.h"


Client::Client(int socket)
{
    _socket = socket;
}

int Client::exec()
{
    timeval tv
    {
        tv.tv_sec = SOCKET_TIMEOUT,
        tv.tv_usec = 0
    };
    fd_set readfds;

    while (state() == State::Connected)
    {
        char buffer[BUFSIZ];

        FD_ZERO(&readfds);
        FD_SET(_socket, &readfds);

        if (select(_socket + 1, &readfds, NULL, NULL, &tv) == -1)
        {
            LOG_ERROR_STRING
            break;
        }

        if (FD_ISSET(_socket, &readfds))
        {
            auto recv_bytes = recv(_socket, (void *)buffer, BUFSIZ - 1, 0);
            if (recv_bytes <= 0)
                break;

            buffer[recv_bytes] = '\0';

            Logger::write(hostInfo(_socket)
                      + " - "
                      + "<< "
                      + buffer);

            if (send(_socket, (const void *)buffer, recv_bytes, 0) <= 0)
                break;

            Logger::write(hostInfo(_socket)
                      + " - "
                      + ">> "
                      + buffer);
        }
    }
    setState(State::Unconnected);

    return EXIT_SUCCESS;
}
