#pragma once

#include <cstring>
#include <cstdlib>
#include <thread>
#include <vector>
#include <algorithm>

#include "lib.h"

class Server
{
public:
    Server(int port);
    ~Server();

    void run();

private:
    void clearDisconnectedSock();

    int _port;
    int _socket;


    std::vector<Client*> _clients;
};

