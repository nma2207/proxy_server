#pragma once

#include <vector>
#include <map>
#include <poll.h>
#include <string>
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
    int _port;
    int _listenSocket;

    std::string _serverIp;
    int _serverPort;

    std::vector<pollfd> _pollSet;
    std::vector<Session> _sessions;
    std::map<int, ConnectionType> _connectionTypes;
    };

