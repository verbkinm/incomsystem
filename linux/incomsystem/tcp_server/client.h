#pragma once

#include <thread>

#include "socket.h"
#include "logger.h"

class Client : public Socket
{
public:
    Client(int socket);

    int exec() override;
};
