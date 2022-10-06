#pragma once

#include <vector>
#include <map>
#include <poll.h>
#include <string>
#include "postgresql_manager.h"
class Server
{
public:
    enum ConnectionType {
        ProxyClient,
        ProxyServer
    };
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

    int _port;
    int _listenSocket;

    std::string _serverIp;
    int _serverPort;

    std::vector<pollfd> _pollSet;
    std::vector<Session> _sessions;
    std::map<int, ConnectionType> _connectionTypes;

    PostgreSqlManager psqlManager;
    static const int BUFFER_SIZE;
    static const std::string LOG_FILENAME;
};

