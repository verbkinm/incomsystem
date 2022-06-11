#pragma once

#include <thread>

#include "socket.h"
#include "logger.h"

class Client : public Socket
{
public:
    Client(int inSocket, int outSocket);
    ~Client();

    int exec() override;

private:
    // сокеты для связи клиента  - прокси сервера и прокси сервара - сервера
    int &_inSocket;
    int _outSocket;
};
