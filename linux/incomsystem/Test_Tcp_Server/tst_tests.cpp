#include <QtTest>

#include "client.h"
#include "server.h"
#include "simplyclient.h"

class Tests : public QObject
{
    Q_OBJECT

public:
    Tests();
    ~Tests();

    uint64_t _listenPort;

private slots:
    void HostInfo();

    void ClientTestState();
    void ServerTestState();

    void ClientConnecedToServer();
    void ServerAddressAlreadyInUse();

    void CountSession();
};

Tests::Tests() : _listenPort(2000)
{
    Logger::init(Logger::FILE);
}

Tests::~Tests()
{

}

void Tests::HostInfo()
{
    Server server(_listenPort);
    std::thread t1(&Server::exec, std::ref(server));

    SimplyClient client("localhost", _listenPort);
    std::thread t2(&SimplyClient::exec, std::ref(client));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    QVERIFY2(client.state() == Socket::State::Connected, "Состояние установлено не верно");

    QVERIFY2(server.hostInfo(client.getSocket()) == "localhost 127.0.0.1:2000", "Не верная информация хоста");

    client.stop();
    server.stop();

    t1.join();
    t2.detach();
}

void Tests::ClientTestState()
{
    Client client(-1);
    QVERIFY2(client.state() == Socket::State::Connected, "Клиент не запущен");
    client.exec();
    QVERIFY2(client.state() == Socket::State::Unconnected, "Клиент не остановлен");
}

void Tests::ServerTestState()
{
    Server server(_listenPort);
    QVERIFY2(server.state() == Socket::State::Unconnected, "Сервер не остановлен");

    std::thread t1(&Server::exec, std::ref(server));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    QVERIFY2(server.state() == Socket::State::Listening, "Сервер на запустился");
    server.stop();
    std::this_thread::sleep_for(std::chrono::seconds(4));

    QVERIFY2(server.state() == Socket::State::Unconnected, "Сервер не остановлен");

    t1.join();
}

void Tests::ClientConnecedToServer()
{
    Server server(_listenPort);
    std::thread t1(&Server::exec, std::ref(server));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    QVERIFY2(server.state() == Socket::State::Listening, "Сервер на запустился");

    SimplyClient client("localhost", _listenPort);
    std::thread t2(&SimplyClient::exec, std::ref(client));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    QVERIFY2(client.state() == Socket::State::Connected, "Клиент не запущен");

    client.stop();
    server.stop();

    t1.join();
    t2.detach();
}

void Tests::ServerAddressAlreadyInUse()
{
    Server server1(_listenPort);
    std::thread t1(&Server::exec, std::ref(server1));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    QVERIFY2(server1.state() == Socket::State::Listening, "Сервер не запустился");

    Server server2(_listenPort);
    std::thread t2(&Server::exec, std::ref(server2));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    QVERIFY2(server2.state() == Socket::State::Unconnected, "Сервер запустился");

    std::this_thread::sleep_for(std::chrono::seconds(4));

    server1.stop();
    server2.stop();

    t1.join();
    t2.join();
}

void Tests::CountSession()
{
    Server server(_listenPort);
    QVERIFY2(server.session() == 0, "Кол-во сессий не совпадает");

    std::thread t1(&Server::exec, std::ref(server));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    QVERIFY2(server.state() == Socket::State::Listening, "Сервер не запустился");

    SimplyClient client("localhost", _listenPort);

    std::thread t2(&SimplyClient::exec, std::ref(client));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    QVERIFY2(client.state() == Socket::State::Connected, "Клиент не запущен");

    QVERIFY2(client.session() == 1, "Кол-во сессий не совпадает");
    client.stop();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    QVERIFY2(client.session() == 0, "Кол-во сессий не совпадает");

    QVERIFY2(server.session() == 0, "Кол-во сессий не совпадает");
    server.stop();

    t1.join();
    t2.detach();
}

QTEST_APPLESS_MAIN(Tests)

#include "tst_tests.moc"
