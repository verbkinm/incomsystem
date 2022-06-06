#include "proxyserver.h"

ProxyServer::ProxyServer(const std::string &serverHost, uint64_t serverPort, uint64_t listenPort) :
    _server_host(serverHost),
    _server_port(serverPort),
    _listen_port(listenPort)
{

}

int ProxyServer::exec()
{
    char buffer[BUFSIZ];

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    std::cout << "Starting proxy server on the port " << _listen_port
              << ", proxy to " << _server_host << ':' << _server_port << "...\n";

    if (_socket == -1)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        return EXIT_FAILURE;
    }

    int enable = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(_listen_port),
    };

    addr.sin_addr.s_addr = INADDR_ANY;
    int addrlen = sizeof(addr);

    if (bind(_socket, (const sockaddr*)&addr, sizeof(addr)) != 0)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        return EXIT_FAILURE;
    }

    if (listen(_socket, 0) < 0)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        return EXIT_FAILURE;
    }
    setState(State::Listening);

    std::cout << "Running proxy server...\n";

    timeval tv
    {
        tv.tv_sec = 3,
        tv.tv_usec = 0
    };
    fd_set readfds;

    while(state() == State::Listening)
    {
        FD_ZERO(&readfds);
        FD_SET(_socket, &readfds);

        if (select(_socket + 1, &readfds, NULL, NULL, &tv) == -1)
        {
            std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
            break;
        }

        if (FD_ISSET(_socket, &readfds))
        {
            int client_sock;
            if ( (client_sock = accept(_socket, (sockaddr *)&addr, (socklen_t*)&addrlen)) < 0)
            {
                std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
                setState(State::Closing);
                continue;
            }

//            std::thread th(general::clientThread, client_sock);
//            th.detach();
        }
    }
    setState(State::Unconnected);

    return EXIT_SUCCESS;
}
