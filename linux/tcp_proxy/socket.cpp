#include "socket.h"

Socket::Socket() : _state(Unconnected)
{

}

Socket::~Socket()
{
    shutdown(_socket, SHUT_RDWR);
    close(_socket);
}

Socket::State Socket::state() const
{
    return _state;
}

void Socket::setState(State newState)
{
    std::lock_guard lg(state_mutex);
    _state = newState;
}
