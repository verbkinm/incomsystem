#pragma once

#include <thread>

#include "baseserver.h"
#include "client.h"

class Server : public BaseServer
{
public:
    Server(int port);

    int exec() override;

private:
    // метод для создания новых подключенных клиентов
    void clientThread(int socket);
};

