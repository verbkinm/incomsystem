#pragma once

#include <mutex>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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


protected:
    // Хранение самого сокета
    int _socket;
    // Состояние сокета
    State _state;

    // Таймаут, используется для предотвращения постоянной блокировки на функциях recv
    const int SOCKET_TIMEOUT;

private:
    // мьютекс для переключения состояний сокета
    std::mutex state_mutex;
};
