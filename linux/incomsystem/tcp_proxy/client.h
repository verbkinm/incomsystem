#pragma once

#include "socket.h"
#include "logger.h"

class Client : public Socket
{
public:
    Client(int inSocket, int outSocket);
    ~Client();

    int exec() override;

private:
    int &_inSocket;
    int _outSocket;
};
