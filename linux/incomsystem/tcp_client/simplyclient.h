#pragma once

#include <thread>

#include "logger.h"
#include "socket.h"

class SimplyClient : public Socket
{
public:
    SimplyClient(const std::string &host, uint64_t port);

    virtual int exec() override;

private:
    std::string _host;
    uint64_t _port;

    // функция для ожидания приёма данных
    void getAnswer();
    // функция для отправки данных
    void sendRequest();
};

