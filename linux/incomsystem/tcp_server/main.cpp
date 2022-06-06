#include <signal.h>

#include "server.h"

std::unique_ptr<Server> server;

void signalHandler(int signal);
void atExitFunc();

bool checkArgs(int argc, const char * const argv[], uint64_t &listenPort);

int main(int argc, char const *argv[])
{
    atexit(atExitFunc);
    signal(SIGINT, signalHandler);

    Logger::init();

    uint64_t listenPort;

    if (!checkArgs(argc, argv, listenPort))
        return EXIT_FAILURE;

    server = std::make_unique<Server>(listenPort);

    return server->exec();
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
        exit(0);
}

void atExitFunc()
{
    server.reset();
}

bool checkArgs(int argc, const char * const argv[], uint64_t &listenPort)
{
    std::string msg = "Usage: " + std::string(argv[0]) + " <listen port>";
    if (argc != 2)
    {
        Logger::write(msg);
        return false;
    }

    try
    {
        listenPort = std::stoull(argv[1]);
    }
    catch (...)
    {
        Logger::write(msg);
        return false;
    }

    return true;
}
