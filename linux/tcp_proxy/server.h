#pragma once

#include <thread>

#include "lib.h"
#include "socket.h"

class Server : public Socket
{
public:
    Server(int port);

    int exec() override;

private:
    int _port;
};

