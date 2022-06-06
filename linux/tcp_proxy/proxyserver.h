#pragma once

#include <thread>

#include "lib.h"
#include "socket.h"

class ProxyServer : public Socket
{
public:
    ProxyServer(const std::string &serverHost, uint64_t serverPort, uint64_t listenPort);

    int exec() override;

private:
    std::string _server_host;
    uint64_t _server_port, _listen_port;
};
