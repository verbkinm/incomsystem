#include <signal.h>

#include "proxyserver.h"

std::unique_ptr<ProxyServer> proxyServer;

void signalHandler(int signal);
void atExitFunc();

bool checkArgs(int argc, const char * const argv[], std::string &serverHost, uint64_t &serverPort, uint64_t &listenPort);

int main(int argc, char const *argv[])
{
    atexit(atExitFunc);
    signal(SIGINT, signalHandler);

    Logger::init();

    std::string serverHost;
    uint64_t serverPort, listenPort;

    if (!checkArgs(argc, argv, serverHost, serverPort, listenPort))
        return EXIT_FAILURE;

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
}

bool checkArgs(int argc, const char * const argv[], std::string &serverHost, uint64_t &serverPort, uint64_t &listenPort)
{
    std::string msg = "Usage: " + std::string(argv[0]) + " <server host> <server port> <listen port>";
    if (argc != 4)
    {
        Logger::write(msg);
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
        Logger::write(msg);
        return false;
    }

    return true;
}
