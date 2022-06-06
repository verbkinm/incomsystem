#include "lib.h"

std::string general::currentDateTime()
{
    time_t now = time(0);
    tm tstruct;
    char buf[32];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

std::string general::hostInfo(int sock)
{
    std::string result;

    sockaddr_in src_addr = {0};
    socklen_t src_addr_len = sizeof(sockaddr_in);
    char client_address_buf[INET_ADDRSTRLEN];

    if (getpeername(sock, (sockaddr *)&src_addr, (socklen_t*)&src_addr_len) != 0)
        return { std::to_string(sock)};

    char buff_client_name[BUFSIZ];
    getnameinfo((sockaddr *)&src_addr, sizeof(src_addr), buff_client_name, BUFSIZ, nullptr, 0, NI_NAMEREQD);

    return std::string(buff_client_name)
            + ' '
            + inet_ntop(AF_INET, &src_addr.sin_addr, client_address_buf, BUFSIZ)
            + ":"
            + std::to_string(ntohs(src_addr.sin_port));
}

void general::thread_cout(const std::string &str)
{
    std::lock_guard<std::mutex> lg(mutex_console);
    std::cout << str;
}

void general::clientThread(int socket)
{
    general::thread_cout(general::currentDateTime()
                         + ' '
                         + general::hostInfo(socket)
                         + " - connected\n");
    Client client(socket);
    client.setState(Socket::State::Connected);
    client.exec();
    general::thread_cout(general::currentDateTime()
                         + ' '
                         + general::hostInfo(socket)
                         + " - disconnected\n");
}
