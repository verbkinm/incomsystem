#include "client.h"

Client::Client(int socket)
{
    _socket = socket;
    _state = State::Connected;
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
        FD_SET(_socket, &readfds);

        // таймаут для recv
        if (select(_socket + 1, &readfds, NULL, NULL, &tv) == -1)
        {
            LOG_ERROR_STRING;
            break;
        }

        if (FD_ISSET(_socket, &readfds))
        {
            // получение данных
            auto recv_bytes = recv(_socket, (void *)buffer, BUFSIZ - 1, 0);
            if (recv_bytes <= 0)
                break;

            buffer[recv_bytes] = '\0';

            Logger::write(hostInfo(_socket)
                      + " - "
                      + "<< "
                      + buffer);

            // отправка данных
            if (send(_socket, (const void *)buffer, recv_bytes, 0) <= 0)
                break;

            Logger::write(hostInfo(_socket)
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
