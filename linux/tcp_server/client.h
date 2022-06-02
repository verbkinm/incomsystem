#pragma once
class Client
{
public:
    Client(int socket);

    void exec();

private:
    int _socket;
};
