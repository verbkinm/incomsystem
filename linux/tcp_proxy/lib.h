#pragma once

#include <mutex>
#include <iostream>

#include <arpa/inet.h>

#include "client.h"

static std::mutex mutex_console;

namespace general
{
    std::string currentDateTime();
    std::string hostInfo(int sock);

    void thread_cout(const std::string &str);
    void clientThread(int socket);
}

