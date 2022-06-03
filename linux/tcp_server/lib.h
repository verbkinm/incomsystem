#pragma once

#include <string>
#include <mutex>
#include <iostream>
#include <memory>

#include "client.h"

static std::mutex mutex_console;

class Client;

namespace general
{
    std::string currentDateTime();
    std::string hostInfo(int sock);

    void thread_cout(const std::string &str);
    void clientThread(Client *client, int socket);
}

