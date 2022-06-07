#include "client.h"

Client::Client(int inSocket, int outSocket) :
    _inSocket(_socket),
    _outSocket(outSocket)
{
    _socket = inSocket;
}

Client::~Client()
{
    shutdown(_outSocket, SHUT_RDWR);
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
        FD_SET(_inSocket, &readfds);

        if (select(_inSocket + 1, &readfds, NULL, NULL, &tv) == -1)
        {
            LOG_ERROR_STRING
            break;
        }

        if (FD_ISSET(_inSocket, &readfds))
        {
            auto bytes = recv(_inSocket, (void *)buffer, BUFSIZ - 1, 0);
            if (bytes <= 0)
                break;

            buffer[bytes] = '\0';

            Logger::write(hostInfo(_inSocket)
                      + " - "
                      + "<< "
                      + buffer);

            bytes = send(_outSocket, (const void *)buffer, bytes, 0);
            if (bytes <= 0)
                break;

            buffer[bytes] = '\0';

            Logger::write(hostInfo(_outSocket)
                      + " - "
                      + ">> "
                      + buffer);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    setState(State::Unconnected);

    return EXIT_SUCCESS;
}
