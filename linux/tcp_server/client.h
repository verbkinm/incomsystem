#pragma once

#include <string>
#include <iostream>
#include <netdb.h>
#include <unistd.h>

#include "lib.h"

class Client
{
public:
    Client(int &socket);
    ~Client();

    int socket() const;

    void exec() const;

private:
    int &_socket;
};
