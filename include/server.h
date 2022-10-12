#pragma once

#include <vector>
#include <map>
#include <poll.h>
#include <string>
#include "postgresql_manager.h"
#include <list>

/**
 * @brief The Server class
 */
class Server
{
public:
    /**
     * @brief Тип сокета или клиент или сервер
     */
    enum ConnectionType {
        ProxyClient,
        ProxyServer
    };

    /**
     * @brief Чтобы хранить соответствие к какому сокету клиента какой сокет сервера подключен
     */
    struct Session {
        int clientSock;
        int serverSock;
    };

    Server(int port, const std::string& serverIp, int serverPort);
    bool start();
    bool run();
    void close();

private:
    int createServerSock();
    void closeConnection(int clientSock);
    std::vector<Session>::iterator findByClient(int);
    std::vector<Session>::iterator findByServer(int);
    std::vector<pollfd>::iterator findInPoll(int);
    void writeToFile(const std::string&);
    // пор на котором сидит прокси сервер
    int _port;
    int _listenSocket;

    // ip postgresql
    std::string _serverIp;
    // порт postgresql
    int _serverPort;

    // хранятся все дескрипторы, которые надо слушать в poll
    std::vector<pollfd> _pollSet;

    // хранятся соответствия клиента к серверу
    std::vector<Session> _sessions;
    //храним у какого соекта какой тип соединения
    std::map<int, ConnectionType> _connectionTypes;
    // парсер psql пакетов
    PostgreSqlManager psqlManager;
    static const int BUFFER_SIZE; // размер буфера который мы считываем с сокетов
    static const std::string LOG_FILENAME; // название лог файла куда мы пишем
};

