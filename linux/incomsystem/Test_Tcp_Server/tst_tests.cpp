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
    void initTestCase();

private slots:
    void HostInfo();
    void ClientTestState();
    void ServerTestState();
    void ClientConnecedToServer();
    void ServerAddressAlreadyUse();
};

Tests::Tests()
{

}

Tests::~Tests()
{

}

void Tests::initTestCase()
{
   Logger::init();
}

void Tests::HostInfo()
{
    uint64_t port = 2000;
    Server server(port);
    std::thread t1(&Server::exec, std::ref(server));

    SimplyClient client("localhost", port);
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
    QVERIFY2(client.state() == Socket::State::Connected, "Состояние установлено не верно");
    client.exec();
    QVERIFY2(client.state() == Socket::State::Unconnected, "Состояние установлено не верно");
}

void Tests::ServerTestState()
{
    Server server(2000);
    QVERIFY2(server.state() == Socket::State::Unconnected, "Состояние установлено не верно");
    std::thread t1(&Server::exec, std::ref(server));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    QVERIFY2(server.state() == Socket::State::Listening, "Состояние установлено не верно");
    server.stop();
    std::this_thread::sleep_for(std::chrono::seconds(4));
    QVERIFY2(server.state() == Socket::State::Unconnected, "Состояние установлено не верно");
    t1.join();
}

void Tests::ClientConnecedToServer()
{
    uint64_t port = 2000;
    Server server(port);
    std::thread t1(&Server::exec, std::ref(server));

    SimplyClient client("localhost", port);
    std::thread t2(&SimplyClient::exec, std::ref(client));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    QVERIFY2(client.state() == Socket::State::Connected, "Состояние установлено не верно");

    client.stop();
    server.stop();

    t1.join();
    t2.detach();
}

void Tests::ServerAddressAlreadyUse()
{
    Server server1(2000);
    std::thread t1(&Server::exec, std::ref(server1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    QVERIFY2(server1.state() == Socket::State::Listening, "Сервер не запустился");

    Server server2(2000);
    std::thread t2(&Server::exec, std::ref(server2));

    std::this_thread::sleep_for(std::chrono::seconds(1));
    QVERIFY2(server2.state() == Socket::State::Unconnected, "Сервер запустился");

    std::this_thread::sleep_for(std::chrono::seconds(3));

    server1.stop();
    server2.stop();

    t1.join();
    t2.join();
}

QTEST_APPLESS_MAIN(Tests)

#include "tst_tests.moc"
