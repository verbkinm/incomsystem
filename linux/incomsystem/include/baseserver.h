#pragma once

#include "socket.h"
#include "logger.h"

class BaseServer : public Socket
{
public:
    BaseServer();
    virtual ~BaseServer() = default;

protected:
    // порт, на котором сервер будет принимать соединения
    int _listenPort;

    int listenSocketCreate();
};

