#include "proxyserver.h"

ProxyServer::ProxyServer(const std::string &serverHost, uint64_t serverPort, uint64_t listenPort) :
    _server_host(serverHost),
    _server_port(serverPort)
{
    _listenPort = listenPort;
}

int ProxyServer::exec()
{
    sockaddr_in server_addr;
    timeval tv;
    fd_set readfds;

    // создание сокета ip4 TCP, на котором будет слушать прокси сервер
    if ((_socket = listenSocketCreate()) == -1)
        return EXIT_FAILURE;

    Logger::write("Starting proxy server on the port "
                  + std::to_string(_listenPort)
                  + ", proxy to "
                  + _server_host
                  + ':'
                  + std::to_string(_server_port)
                  + "...");

    // наполнение структы sockaddr_in по переданному параметру в консоли server_host
    if (serverAddrCreate(server_addr) == -1)
        return EXIT_FAILURE;

    Logger::write("Running proxy server...");

    tv =
    {
        tv.tv_sec = SOCKET_TIMEOUT,
        tv.tv_usec = 0
    };

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
            // сокеты для связи с клиентом и сервером
            int inSocket, outSocket;

            // ожидание подключения новых клиентов
            if ((inSocket = inSocketCreate()) == -1)
                continue;

            // создание сокета ip4 TCP для соединения с сервером
            if ((outSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                LOG_ERROR_STRING;
                shutdown(inSocket, SHUT_RDWR);
                continue;
            }

            // соединение с сервером
            if (connect(outSocket, (const sockaddr* const)&server_addr, sizeof(server_addr)) != 0)
            {
                Logger::write("Error connect to "
                              + _server_host
                              + ":"
                              + std::to_string(_server_port));
                shutdown(inSocket, SHUT_RDWR);
                shutdown(outSocket, SHUT_RDWR);
                continue;
            }

            // поток для нового подключенного клиента, клиент - прокси сервер
            std::thread th1(&ProxyServer::clientThread, this, inSocket, outSocket);
            // поток для нового подключенного клиента, прокси сервер - сервер
            std::thread th2(&ProxyServer::clientThread, this, outSocket, inSocket);

            th1.detach();
            th2.detach();
        }
        // задержка используется для разгрузки процессора
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    _state = State::Unconnected;

    return EXIT_SUCCESS;
}

int ProxyServer::inSocketCreate()
{
    sockaddr_in addr;

    socklen_t addrlen = sizeof(addr);
    // ожидание подключения новых клиентов
    auto sock = accept(_socket, (sockaddr *)&addr, &addrlen);

    if (sock < 0)
    {
        LOG_ERROR_STRING;
        _state = State::Closing;
        return -1;
    }

    return sock;
}

int ProxyServer::serverAddrCreate(sockaddr_in &addr) const
{
    // метод для наполнения структуры sockaddr_in, если получилось получить
    // имя сервера по ведённому параметру server_host

    const hostent *remote_host { gethostbyname(_server_host.c_str()) };
    if (remote_host == nullptr)
    {
        Logger::write(_server_host + " - unknow host name or ip");
        return -1;
    }

    addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(_server_port)
    };

    addr.sin_addr.s_addr = *(const in_addr_t*)remote_host->h_addr;

    return 0;
}

void ProxyServer::clientThread(int inSocket, int outSocket)
{
    _session++;
    // сохраняем информацию о сокете
    auto hInfo = hostInfo(inSocket);

    Logger::write(hInfo + " - connected");

    // объект клиент, состояние - подключен, так как сокет уже был создан
    Client client(inSocket, outSocket);
    client.exec();

    Logger::write(hInfo + " - disconnected");
    _session--;
}

