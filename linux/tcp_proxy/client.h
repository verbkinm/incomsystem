#pragma once

#include <string.h>

#include "lib.h"
#include "socket.h"

class Client : public Socket
{
public:
    Client(int socket);

    int exec() override;
};
