#pragma once

#include <thread>

#include "client.h"

class Server : public Socket
{
public:
    Server(int port);

    int exec() override;

private:
    // порт, на котором сервер будет принимать соединения
    int _port;

    // метод для создания новых подключенных клиентов
    void clientThread(int socket) const;
};

