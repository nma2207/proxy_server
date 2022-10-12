#include "server.h"

#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <algorithm>
#include <linux/tcp.h>
#include <fcntl.h>

const int Server::BUFFER_SIZE=20000;
const std::string Server::LOG_FILENAME = "query.txt";

Server::Server(int port, const std::string& serverIp, int serverPort)
    : _port{port}
    , _serverIp{serverIp}
    , _serverPort{serverPort}
{

}

bool Server::start()
{
    _listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenSocket == -1) {
        std::cerr << "get socket error" << std::endl;
        return false;
    }

    int turnOn=1;
    if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &turnOn, sizeof(turnOn)) == -1) {
        std::cerr << "set reuse optioin error" << std::endl;
        return false;
    }

    if (fcntl(_listenSocket,F_SETFL, fcntl(_listenSocket, F_GETFD,0) | O_NONBLOCK) < 0) {
        std::cerr << "set nonblocking error" << std::endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serverAddr.sin_port = htons(_port);

    if (bind(_listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1 ) {
        std::cerr << "bind server error" << std::endl;
        return false;
    }


    if (listen(_listenSocket, 1000) == -1) {
        std::cerr << "socket listen error" << std::endl;
        return false;
    }

    std::cout << "PROXY server started successfully at port"<<_port << std::endl;
    std::cout << "Postgresql address: "<<_serverIp << ":" <<_serverPort << std::endl;
    return true;
}

bool Server::run()
{
    pollfd listenPollFd;
    listenPollFd.fd = _listenSocket;
    listenPollFd.events = POLLIN;
    _pollSet.reserve(1000);
    _sessions.reserve(1000);
    _pollSet.push_back(listenPollFd);



    while(true) {
        std::cout << "Wait for poll " <<_pollSet.size()<<" "<<_sessions.size()<<" "<< _connectionTypes.size()<< std::endl;
        int pollRes = poll(&_pollSet[0], _pollSet.size(), -1);

        if (pollRes < 0) {
            std::cerr << "Poll error";
            return false;
        }
        else if (pollRes == 0){
            std::cerr << "timeout error" << std::endl;
            return false;
        }
        //std::cout << "start poll " << pollRes <<" "<< _pollSet.size() <<std::endl;


        int n = _pollSet.size();
        int i=0;
        for(auto it = _pollSet.begin();i<n && it != _pollSet.end(); it++, i++) {
            //std::cout <<"revents "<<i << " "<<it->revents  << " " << it->fd<<std::endl;
            if (it->revents==0) {

                continue;
            }

            if (it->revents == POLLIN) {
                if (it->fd == _listenSocket) {
                    int clientSock = accept(_listenSocket, NULL, NULL);
                    if (clientSock == -1) {
                        if (errno == EWOULDBLOCK)
                            continue;
                        std::cerr << "client accept failed" << std::endl;
                        continue;
                    }

                    _sessions.push_back(Session{clientSock, 0});
                    _connectionTypes[clientSock] = ProxyClient;

                    pollfd clientPollFd;
                    clientPollFd.fd = clientSock;
                    clientPollFd.events = POLLIN;
                    _pollSet.push_back(clientPollFd);
                    std::cout <<"add new client, client count: " << _pollSet.size() - 1 << " "<<clientSock<< std::endl;

                }
                else {
                    //std::cout << "got mess" << std::endl;
                    char message[Server::BUFFER_SIZE];
                    memset(message, 0, Server::BUFFER_SIZE);
                    int nbytes=0;
                    int nread=0;
                     while ((nread=recv(it->fd, message+nbytes, Server::BUFFER_SIZE-nbytes, MSG_DONTWAIT))>0){
                         nbytes+=nread;
                     }
                    //std::cout << "nbytes "<<nbytes << std::endl;
                    if (nbytes < 0) {
                        ConnectionType type = _connectionTypes[it->fd];


                        if (type == ProxyClient) {
                            std::cout <<"close 1 " <<nbytes << std::endl;
                            closeConnection(it->fd);
                        }
                        else {

                            auto sessIt = findByServer(it->fd);
                            std::cout <<"close 2 "<< nbytes <<" "<< sessIt->clientSock <<std::endl;
                            closeConnection(sessIt->clientSock);
                        }

                    }

                    else if (nbytes>0){
                        //std::cout <<"recive:" << message;
                        ConnectionType type = _connectionTypes[it->fd];
                        std::vector<Session>::iterator sessIt;
                        if (type == ProxyClient) {
                            sessIt = findByClient(it->fd);
                        }
                        else if (type == ProxyServer) {
                            sessIt = findByServer(it->fd);
                        }

                        if (type == ProxyClient) {
                            if (sessIt->serverSock == 0) {
                                int serverSock = this->createServerSock();
                                _connectionTypes[serverSock] = ProxyServer;
                                if (serverSock < 0) {
                                    std::cerr << "Server connection error" << std::endl;
                                    return false;
                                }
                                sessIt->serverSock=serverSock;
                                pollfd pfd;
                                pfd.fd=serverSock;
                                pfd.events=POLLIN;
                                _pollSet.push_back(pfd);
                                std::cout << "add server "<<sessIt->clientSock<<" " <<serverSock << std::endl;
                            }

                            int sendBytes = send(sessIt->serverSock, message, nbytes, MSG_NOSIGNAL);

                            if (sendBytes < 0) {
                                std::cerr << "client send to server error "<<sessIt->clientSock << " " <<sessIt->serverSock<<" "<<sendBytes<<" " << errno<< std::endl;
                                closeConnection(sessIt->clientSock);
                            }
                            //std::cout << "sended bytes to server" << std::endl;
                            std::string parserRes = psqlManager.parseF(message, nbytes);
                            writeToFile(parserRes);

                            if (message[0] == 'X') {// terminate
                                std::cout <<"terminate " <<sessIt->clientSock<<std::endl;
                                closeConnection(sessIt->clientSock);
                            }
                        }
                        else if (type == ProxyServer && sessIt->clientSock > 0) {
                            int sendBytes = send(sessIt->clientSock, message, nbytes, MSG_NOSIGNAL);

                            if (sendBytes < 0) {
                                std::cerr << "server send to client error" << std::hex << errno<<std::dec<< std::endl;

                                closeConnection(sessIt->clientSock);
                            }
                            //std::cout << "sended bytes to client" << std::endl;
                            std::string parserRes = psqlManager.parseB(message, nbytes);
                            writeToFile(parserRes);
                        }
                    }
                }
            }
            else {
                //std::cout <<"err" << i << std::endl;
//                if (it->fd == _listenSocket) {
//                    std::cerr << "socket listen error" << std::endl;
//                }
//                else {
//                    std::cout << "disconnect client " <<it->fd<<" "<< std::hex << it->revents << std::endl;
//                    if (it->fd != 0)
//                        closeConnection(it->fd);
//                }
            }
        }

        //std::cout << std::endl;
        _pollSet.erase(std::remove_if(_pollSet.begin(), _pollSet.end(), [](pollfd pfd){
            return pfd.fd == -1;
        }), _pollSet.end());


    }
}

void Server::close()
{
    for(Session s: _sessions){
        //std::cout <<"close 3" << std::endl;
        closeConnection(s.clientSock);
    }

    for(pollfd p: _pollSet)
        ::close(p.fd);

    _pollSet.clear();
    _sessions.clear();
    _connectionTypes.clear();

}

int Server::createServerSock()
{
    sockaddr_in addr;
    int turnOn=1;
    int sock = socket(AF_INET, SOCK_STREAM, 0 );
    if (sock < 0) {
        std::cerr << "cannot create server socket";
        return sock;
    }
    int rc;
    rc = setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR, (char *)&turnOn, sizeof(turnOn));
    if (rc < 0)
    {
        std::cerr << "setsockopt() failed";
        return rc;
    }


    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &turnOn, sizeof(int)) <0){
        std::cerr << "setsockopt(TCP_NODELAY) failed";
        return -1;
    }

    rc = fcntl(sock,F_SETFL, fcntl(sock, F_GETFD,0) | O_NONBLOCK);
    if (rc < 0) {
        std::cerr << "set nonblock error" << std::endl;
        return rc;
    }
    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr = inet_addr(_serverIp.data());

    addr.sin_port=htons(_serverPort);
    rc = connect(sock, (sockaddr*)&addr, sizeof(addr));
    //rc = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0 && errno != EINPROGRESS) {
        std::cerr << "connect to server error "<<rc << std::endl;
        std::cout << errno <<std::endl;
        return rc;
    }


    return sock;

}

void Server::closeConnection(int clientSock)
{
    std::cout << "close connection" << clientSock << std::endl;
    ::close(clientSock);
    auto sessionIt = findByClient(clientSock);
    int serverSock = sessionIt->serverSock;

    _sessions.erase(sessionIt);
    _connectionTypes.erase(clientSock);
    findInPoll(clientSock)->fd = -1;


    if (serverSock > 0) {
        std::cout << "close connection" << sessionIt->serverSock << std::endl;
        ::close(serverSock);
        _connectionTypes.erase(serverSock);
        findInPoll(serverSock)->fd=-1;
    }
}

std::vector<Server::Session>::iterator Server::findByClient(int clientSock)
{
    return std::find_if(_sessions.begin(), _sessions.end(), [&clientSock](Session s) {
                                    return s.clientSock == clientSock;
                                });
}

std::vector<Server::Session>::iterator Server::findByServer(int serverSock)
{
    return std::find_if(_sessions.begin(), _sessions.end(), [&serverSock](Session s) {
                                    return s.serverSock == serverSock;
    });
}

std::vector<pollfd>::iterator Server::findInPoll(int sock)
{
    return std::find_if(_pollSet.begin(), _pollSet.end(), [&sock](pollfd p) {
                                    return p.fd == sock;
    });
}

void Server::writeToFile(const std::string &str)
{
    std::ofstream logfile;
    logfile.open(Server::LOG_FILENAME, std::ios_base::app);
    if (logfile.is_open()) {
        logfile << str << std::endl;
        logfile.close();
        //std::cout << "Write to file" << str << std::endl;
    }
    else {
        std::cout << "WRITE To FILE ERRRROR" << std::endl;
    }


}
