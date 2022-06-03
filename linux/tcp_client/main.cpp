#include <iostream>
#include <thread>

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstdlib>
#include <csignal>
#include <ctime>

int SOCK = 0;

void get_answer(const int &sock);
void send_request(const int &sock);
bool checkArgs(int argc, const char * const argv[], std::string &host, uint64_t &port);

void signalHandler(int signal);
void atExitFunc();

std::string hostInfo(const int &sock);
std::string currentDateTime();

int main(int argc, const char * const argv[])
{
    std::string host;
    uint64_t port = 0;

    atexit(atExitFunc);
    signal(SIGINT, signalHandler);

    if (!checkArgs(argc, argv, host, port))
        return EXIT_FAILURE;

    if ((SOCK = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        std::cerr << "error socket" << std::endl;
        return EXIT_FAILURE;
    }

    const hostent *remote_host { gethostbyname(host.c_str()) };
    if (remote_host == nullptr)
    {
        std::cerr << host << " - unknow host name or ip" << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in server_addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

    server_addr.sin_addr.s_addr = *reinterpret_cast<const in_addr_t*>(remote_host->h_addr);

    if (connect(SOCK, reinterpret_cast<const sockaddr* const>(&server_addr), sizeof(server_addr)) != 0)
    {
        std::cerr << "Error connect to " << host << ":" << port << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Connected to " << host << ":" << port << std::endl;

    std::thread reciveThread(get_answer, std::ref(SOCK));
    std::thread sendThread(send_request, std::ref(SOCK));

    reciveThread.join();
    sendThread.join();

    return EXIT_SUCCESS;
}

void get_answer(const int &sock)
{
    while (true)
    {
        char buff[BUFSIZ];

        auto recv_bytes = recv(sock, (void *)buff, BUFSIZ - 1, 0);
        if (recv_bytes <= 0)
            exit(EXIT_FAILURE);

        buff[recv_bytes] = '\0';

        std::cout << "\r<< " << currentDateTime() << ' ' << hostInfo(sock)
                  << " - " << buff << "\n>> " << std::flush;
    }
}

void send_request(const int &sock)
{
    std::string buff;

    while (true)
    {
        std::cout << ">> " << std::flush;
        std::getline(std::cin, buff);
        if(buff.length() == 0)
            continue;

        if (send(sock, (const void *)buff.data(), buff.length(), 0) <= 0)
            exit(EXIT_FAILURE);
    }
}

bool checkArgs(int argc, const char * const argv[], std::string &host, uint64_t &port)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return false;
    }

    try
    {
        host = argv[1];
        port = std::stoull(argv[2]);
    }
    catch (...)
    {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return false;
    }

    return true;
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
        exit(0);
}

void atExitFunc()
{
    close(SOCK);
    std::cout << "\nClose socket.\n";
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

std::string currentDateTime()
{
    time_t now = time(0);
    tm tstruct;
    char buf[32];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}
