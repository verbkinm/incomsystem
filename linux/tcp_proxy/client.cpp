#include "client.h"


Client::Client(int socket)
{
    _socket = socket;
}

int Client::exec()
{
    timeval tv
    {
        tv.tv_sec = 3,
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
            std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
            break;
        }

        if (FD_ISSET(_socket, &readfds))
        {
            auto recv_bytes = recv(_socket, (void *)buffer, BUFSIZ - 1, 0);
            if (recv_bytes <= 0)
                break;

            buffer[recv_bytes] = '\0';

            std::cout << general::currentDateTime()
                      << ' '
                      << general::hostInfo(_socket)
                      << " - "
                      << buffer
                      << "\n";

            if (send(_socket, (const void *)buffer, recv_bytes, 0) <= 0)
                break;
        }
    }
    setState(State::Unconnected);

    return EXIT_SUCCESS;
}
