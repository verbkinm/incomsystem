#include "server.h"


Server::Server(int port) : _port(port)
{

}

Server::~Server()
{
    std::cout << "Clients count: " << _clients.size() << std::endl;
    for (auto ptr : _clients)
    {
        if (ptr != nullptr)
        {
            std::cout << "Socket " << ptr->socket() <<std::endl;
            delete ptr;
        }
    }

    close(_socket);
}

void Server::run()
{
    char buffer[BUFSIZ];

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    std::cout << "Starting echo server on the port " << _port << "...\n";

    if (_socket == -1)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        return;
    }

    sockaddr_in addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(_port),
    };

    addr.sin_addr.s_addr = INADDR_ANY;
    int addrlen = sizeof(addr);

    if (bind(_socket, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        return;
    }

    if (listen(_socket, 0) < 0)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
        return;
    }

    std::cout << "Running echo server...\n" << std::endl;

    while(true)
    {
        int client_sock;
        if ( (client_sock = accept(_socket, (sockaddr *)&addr, (socklen_t*)&addrlen)) < 0)
        {
            std::cerr << strerror_r(errno, &buffer[0], BUFSIZ) << std::endl;
            continue;
        }

        Client *client = nullptr;
        _clients.push_back(client);
        std::thread th(general::clientThread, _clients, client_sock);
        th.detach();

        clearDisconnectedSock();

        if (_clients.size() > 4)
            break;
    }
}

void Server::clearDisconnectedSock()
{
//    _clients.erase(std::remove_if(_clients.begin(), _clients.end(), [](int *ptr){
//        if (ptr != nullptr && *ptr == -1)
//        {
//            delete ptr;
//            return true;
//        }
//        return false;
//    }), _clients.end());
}
