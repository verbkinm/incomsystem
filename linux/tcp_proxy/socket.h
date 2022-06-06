#pragma once

#include <mutex>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

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

protected:
    int _socket;

private:
    std::mutex state_mutex;
    State _state;
};
