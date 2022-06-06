#include <signal.h>
#include <thread>

#include "logger.h"
#include "socket.h"

static int SOCKET;

void getAnswer(int sock);
void sendRequest(int sock);
bool checkArgs(int argc, const char * const argv[], std::string &host, uint64_t &port);

void signalHandler(int signal);
void atExitFunc();

int main(int argc, const char * const argv[])
{
    atexit(atExitFunc);
    signal(SIGINT, signalHandler);

    Logger::init();

    std::string host;
    uint64_t port;

    sockaddr_in server_addr;

    if (!checkArgs(argc, argv, host, port))
        return EXIT_FAILURE;

    if ((SOCKET = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        LOG_ERROR_STRING
                return EXIT_FAILURE;
    }

    const hostent *remote_host { gethostbyname(host.c_str()) };
    if (remote_host == nullptr)
    {
        LOG_ERROR_STRING
                return EXIT_FAILURE;
    }

    server_addr =
    {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };
    server_addr.sin_addr.s_addr = *(const in_addr_t*)remote_host->h_addr;

    if (connect(SOCKET, (const sockaddr* const)&server_addr, sizeof(server_addr)) != 0)
    {
        LOG_ERROR_STRING
                return EXIT_FAILURE;
    }

    Logger::write("Connected to " + Socket::hostInfo(SOCKET));

    std::thread reciveThread(getAnswer, SOCKET);
    sendRequest(SOCKET);

    reciveThread.join();

    return EXIT_SUCCESS;
}

void getAnswer(int sock)
{
    char buffer[BUFSIZ];

    while (true)
    {
        auto recv_bytes = recv(sock, (void *)buffer, BUFSIZ - 1, 0);
        if (recv_bytes <= 0)
        {
            Logger::write("Disconnected from " + Socket::hostInfo(sock));
            exit(EXIT_FAILURE);
        }

        buffer[recv_bytes] = '\0';

        Logger::write(Socket::hostInfo(sock)
                      + " - << "
                      + buffer);
    }
}

void sendRequest(int sock)
{
    std::string buffer;

    while (true)
    {
        std::getline(std::cin, buffer);

        if(buffer.length() == 0)
            continue;

        if (send(sock, (const void *)buffer.data(), buffer.length(), 0) <= 0)
            exit(EXIT_FAILURE);

        Logger::write(Socket::hostInfo(sock)
                      + " - >> "
                      + buffer);
    }
}

bool checkArgs(int argc, const char * const argv[], std::string &host, uint64_t &port)
{
    std::string msg = "Usage: " + std::string(argv[0]) + " <host> <port>";
    if (argc != 3)
    {
        Logger::write(msg);
        return false;
    }

    try
    {
        host = argv[1];
        port = std::stoull(argv[2]);
    }
    catch (...)
    {
        Logger::write(msg);
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
    shutdown(SOCKET, SHUT_RDWR);
    close(SOCKET);
}
