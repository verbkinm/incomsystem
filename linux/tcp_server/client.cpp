#include "client.h"

#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstdlib>
#include <csignal>
#include <ctime>

Client::Client(int socket) : _socket(socket)
{

}

std::string currentDateTime()
{
    time_t now = time(0);
    tm tstruct;
    char buf[32];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

std::string hostInfo(const int &sock)
{
    std::string result;

    sockaddr_in src_addr = {0};
    socklen_t src_addr_len = sizeof(sockaddr_in);
    char client_address_buf[INET_ADDRSTRLEN];

    if (getpeername(sock, (sockaddr *)&src_addr, (socklen_t*)&src_addr_len) != 0)
        return {};

    char buff_client_name[BUFSIZ];
    getnameinfo((sockaddr *)&src_addr, sizeof(src_addr), buff_client_name, BUFSIZ, nullptr, 0, NI_NAMEREQD);

    return std::string(buff_client_name) + ' '
            + inet_ntop(AF_INET, &src_addr.sin_addr, client_address_buf, BUFSIZ)
            + ":" + std::to_string(ntohs(src_addr.sin_port));
}

void Client::exec()
{
    while (true)
    {
        char buff[BUFSIZ];

        auto recv_bytes = recv(_socket, (void *)buff, BUFSIZ - 1, 0);
        if (recv_bytes <= 0)
            exit(EXIT_FAILURE);

        buff[recv_bytes] = '\0';

        std::cout << "\r<< " << currentDateTime() << ' ' << hostInfo(_socket)
                  << " - " << buff << "\n>> " << std::flush;

        if (send(_socket, (const void *)buff, recv_bytes, 0) <= 0)
            exit(EXIT_FAILURE);
    }
}

