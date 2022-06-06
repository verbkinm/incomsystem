#pragma once

#include <mutex>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class Socket
{
public:

    enum State
    {
        Unconnected,
        HostLookup,
        Connecting,
        Connected,
        Bound,
        Closing,
        Listening
    };

    Socket();
    virtual ~Socket();

    virtual int exec() = 0;

    State state() const;
    void setState(State newState);

    static std::string hostInfo(int sock);

protected:
    int _socket;

    const int SOCKET_TIMEOUT;

private:
    std::mutex state_mutex;
    State _state;
};
