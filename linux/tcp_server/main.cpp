#include <signal.h>
#include <iostream>

#include "server.h"

std::unique_ptr<Server> server;

void signalHandler(int signal);
void atExitFunc();

int main(int argc, char const *argv[])
{
    atexit(atExitFunc);
    signal(SIGINT, signalHandler);

    uint64_t port;

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        port = std::stoi(argv[1]);
    }
    catch (...)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    server = std::make_unique<Server>(port);

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
    std::cout << "\nExiting...\n";
}
