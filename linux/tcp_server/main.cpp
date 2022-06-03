#include <signal.h>
#include <iostream>

#include "server.h"

Server *server = nullptr;

void signalHandler(int signal);
void atExitFunc();

int main(int argc, char const *argv[])
{
    atexit(atExitFunc);
    signal(SIGINT, signalHandler);

    int port;

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

    server = new Server(port);//std::make_unique<Server>(port);
    server->run();

    return EXIT_SUCCESS;
}

void signalHandler(int signal)
{
    if (signal == SIGINT)
        exit(0);
}

void atExitFunc()
{
    delete server;
//    server.reset();
    std::cout << "\nClose socket.\n";
}
