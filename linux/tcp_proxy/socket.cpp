#include "socket.h"

Socket::Socket() : _state(Unconnected)
{

}

Socket::~Socket()
{
    shutdown(_socket, SHUT_RDWR);
    close(_socket);
}

Socket::State Socket::state() const
{
    return _state;
}

void Socket::setState(State newState)
{
    std::lock_guard lg(state_mutex);
    _state = newState;
}

std::string Socket::hostInfo(int sock)
{
    std::string result;

    sockaddr_in src_addr = {0};
    socklen_t src_addr_len = sizeof(sockaddr_in);
    char client_address_buf[INET_ADDRSTRLEN];

    if (getpeername(sock, (sockaddr *)&src_addr, (socklen_t*)&src_addr_len) != 0)
        return {std::to_string(sock)};

    char buff_client_name[BUFSIZ];
    getnameinfo((sockaddr *)&src_addr, sizeof(src_addr), buff_client_name, BUFSIZ, nullptr, 0, NI_NAMEREQD);

    return std::string(buff_client_name)
            + ' '
            + inet_ntop(AF_INET, &src_addr.sin_addr, client_address_buf, BUFSIZ)
            + ":"
            + std::to_string(ntohs(src_addr.sin_port));
}
