#include "client.h"

Client::Client(int inSocket, int outSocket) :
    _inSocket(_socket),
    _outSocket(outSocket)
{
    _socket = inSocket;
    _state = State::Connected;
}

Client::~Client()
{
    shutdown(_outSocket, SHUT_RDWR);
}

int Client::exec()
{
    // необходимо для функции select
    timeval tv
    {
        tv.tv_sec = SOCKET_TIMEOUT,
        tv.tv_usec = 0
    };
    fd_set readfds;

    // запуск основного цикла клиента
    while (state() == State::Connected)
    {
        // буфер для данных приёма и передачи
        char buffer[BUFSIZ];

        // необходимо для функции select
        FD_ZERO(&readfds);
        FD_SET(_inSocket, &readfds);

        // таймаут для recv
        if (select(_inSocket + 1, &readfds, NULL, NULL, &tv) == -1)
        {
            LOG_ERROR_STRING;
            break;
        }

        if (FD_ISSET(_inSocket, &readfds))
        {
            // получение данных
            auto bytes = recv(_inSocket, (void *)buffer, BUFSIZ - 1, 0);
            if (bytes <= 0)
                break;

            buffer[bytes] = '\0';

            Logger::write(hostInfo(_inSocket)
                      + " - "
                      + "<< "
                      + buffer);

            // отправка данных
            bytes = send(_outSocket, (const void *)buffer, bytes, 0);
            if (bytes <= 0)
                break;

            buffer[bytes] = '\0';

            Logger::write(hostInfo(_outSocket)
                      + " - "
                      + ">> "
                      + buffer);
        }
        // задержка используется для разгрузки процессора
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    _state = State::Unconnected;

    return EXIT_SUCCESS;
}
