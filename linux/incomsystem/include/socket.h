#pragma once

#include <string>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <atomic>

// Абстарктный класс, используется в таких классах как Client, Server, ProxyServer
class Socket
{
public:

    // Перечисления для состояний сокета
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

    // Конструктор и деструктор
    Socket();
    virtual ~Socket();

    // Виртуальный нулевой метод для переопределяния в потомках
    virtual int exec() = 0;
    void stop();

    // Методы получения состояния соединения
    State state() const;

    // получить сокет
    int getSocket() const;

    // Метод получения информации о сокете в виде строки
    static std::string hostInfo(int sock);

    // Метод получения информации о кол-ве сессий
    uint64_t session() const;

protected:
    // Хранение самого сокета
    int _socket;

    // Состояние сокета
    std::atomic<State> _state;

    //кол-во сессий
    std::atomic<uint64_t> _session;

    // Таймаут, используется для предотвращения постоянной блокировки на функциях recv
    const int SOCKET_TIMEOUT;
};
