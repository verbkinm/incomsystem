#pragma once

#include "socket.h"

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
