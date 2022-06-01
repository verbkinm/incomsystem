#include <iostream>
#include <thread>

#include <netdb.h>
#include <unistd.h>

#include <cstdlib>
#include <csignal>

int SOCK = 0;

void get_answer(const int &sock);
void send_request(const int &sock);
bool checkArgs(int argc, const char * const argv[], std::string &host, uint64_t &port);

void signal_handler(int signal);
void atExitFunc();

int main(int argc, const char * const argv[])
{
    std::string host;
    uint64_t port = 0;

    atexit(atExitFunc);
    signal(SIGINT, signal_handler);

    if (!checkArgs(argc, argv, host, port))
        return EXIT_FAILURE;

    if (!(SOCK = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
    {
        std::cerr << "error socket" << std::endl;
        return EXIT_FAILURE;
    }

    const struct hostent *remote_host { gethostbyname(host.c_str()) };
    if (remote_host == nullptr)
    {
        std::cerr << host << " - unknow host name or ip" << std::endl;
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr =
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
        std::cout << "\r<< " << buff << "\n>> " << std::flush;
    }
}

void send_request(const int &sock)
{
    std::string buff;

    while (true)
    {
        std::cout << ">> " << std::flush;
        std::getline(std::cin, buff);
        buff += "\n";

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

void signal_handler(int signal)
{
    if (signal == SIGINT)
        exit(0);
}

void atExitFunc()
{
    close(SOCK);
    std::cout << "\nClose socket.\n";
}
