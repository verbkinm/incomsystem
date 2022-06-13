#pragma once

#include <thread>

#include "baseserver.h"
#include "client.h"

class ProxyServer : public BaseServer
{
public:
    ProxyServer(const std::string &serverHost, uint64_t serverPort, uint64_t listenPort);

    int exec() override;

private:
    std::string _server_host;
    uint64_t _server_port;


    // методы для уменьшения кода в методе exec
    int inSocketCreate();
    int serverAddrCreate(sockaddr_in &addr) const;

    // метод для создания новых подключенных клиентов
    void clientThread(int inSocket, int outSocket);
};
