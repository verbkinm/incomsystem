#include <signal.h>
#include <iostream>

#include "proxyserver.h"

std::unique_ptr<ProxyServer> proxyServer;

void signalHandler(int signal);
void atExitFunc();

bool checkArgs(int argc, const char * const argv[], std::string &serverHost, uint64_t &serverPort, uint64_t &listenPort);

int main(int argc, char const *argv[])
{
    atexit(atExitFunc);
    signal(SIGINT, signalHandler);

    std::string serverHost;
    uint64_t serverPort = 0,
            listenPort = 0;

    if (!checkArgs(argc, argv, serverHost, serverPort, listenPort))
        return EXIT_FAILURE;

    const hostent *remote_host { gethostbyname(serverHost.c_str()) };
    if (remote_host == nullptr)
    {
        std::cerr << serverHost << " - unknow host name or ip" << std::endl;
        return EXIT_FAILURE;
    }

    proxyServer = std::make_unique<ProxyServer>(serverHost, serverPort, listenPort);

    return proxyServer->exec();
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
        exit(0);
}

void atExitFunc()
{
    proxyServer.reset();
    std::cout << "\nExiting...\n";
}

bool checkArgs(int argc, const char * const argv[], std::string &serverHost, uint64_t &serverPort, uint64_t &listenPort)
{
    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " <server host> <server port> <listen port>" << std::endl;
        return false;
    }

    try
    {
        serverHost = argv[1];
        serverPort = std::stoull(argv[2]);
        listenPort = std::stoull(argv[3]);
    }
    catch (...)
    {
        std::cout << "Usage: " << argv[0] << " <server host> <server port> <listen port>" << std::endl;
        return false;
    }

    return true;
}
