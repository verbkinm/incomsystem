#pragma once

#include <thread>

#include "client.h"

class Server : public Socket
{
public:
    Server(int port);

    int exec() override;

private:
    int _port;

    void clientThread(int socket) const;
};
