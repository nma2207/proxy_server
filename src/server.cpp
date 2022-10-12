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

//  инициализация констант
const int Server::BUFFER_SIZE=20000;
const std::string Server::LOG_FILENAME = "query.txt";

Server::Server(int port, const std::string& serverIp, int serverPort)
    : _port{port}
    , _serverIp{serverIp}
    , _serverPort{serverPort}
{

}

/**
 * @brief Создаем сокет, типа стартуем сервер
 */
bool Server::start()
{
    // Создаем неблокирующий сокет к которому будут подключатся клиенты
    _listenSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (_listenSocket == -1) {
        std::cerr << "get socket error" << std::endl;
        return false;
    }

    int turnOn=1;
    if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &turnOn, sizeof(turnOn)) == -1) {
        std::cerr << "set reuse optioin error" << std::endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    // будет на локальной машине с заданным портом
    serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serverAddr.sin_port = htons(_port);

    // привязываем к данному адресу
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
    // основной цикл программы
    pollfd listenPollFd;
    listenPollFd.fd = _listenSocket;
    listenPollFd.events = POLLIN; // обрабатываем события прихода сообщений
    _pollSet.reserve(1000); //резервируем для быстродействия
    _sessions.reserve(1000);
    _pollSet.push_back(listenPollFd); // добавляем слушающий сокет

    while(true) {
        // ждем пока будут подключения, -1 значит что ждем без таймаута до бесконечности, результат - количество дескрипторов с событиями
        int pollRes = poll(&_pollSet[0], _pollSet.size(), -1);

        // если результат меньше нуля значит была ошибка и завершаем работу
        if (pollRes < 0) {
            std::cerr << "Poll error" <<std::endl;
            return false;
        }
        // при таймауте тоже завершаем работу (но его у нас нет)
        else if (pollRes == 0){
            std::cerr << "timeout error" << std::endl;
            return false;
        }


        int n = _pollSet.size();
        int i=0;
        // цикл по всем существующим дескрипторам
        for(auto it = _pollSet.begin();i<n && it != _pollSet.end(); it++, i++) {
            //если нет сообщений то переходим к следующему
            if (it->revents==0) {

                continue;
            }
            // если есть событие, что есть данные
            if (it->revents == POLLIN) {
                if (it->fd == _listenSocket) { // если это слушающий дескриптор
                    int clientSock = accept(_listenSocket, NULL, NULL); // то создаем дескриптор для клиента
                    if (clientSock == -1) { // если номер сокета -1, что значит там ошибка
                        if (errno == EWOULDBLOCK) // значит ресурс временно заблокирован, надо подождать
                            continue;
                        std::cerr << "client accept failed" << std::endl; // если не так, значит не удалось подключить клиента
                        continue;
                    }

                    // если все хорошо, то добавляем клиента
                    _sessions.push_back(Session{clientSock, 0}); // сокет сервера пока 0
                    _connectionTypes[clientSock] = ProxyClient;

                    pollfd clientPollFd;
                    clientPollFd.fd = clientSock;
                    clientPollFd.events = POLLIN;
                    _pollSet.push_back(clientPollFd);
                }
                else {
                    // если сообщение не от самого первого сокета, значит пришло что-то от сервара или клиента
                    char message[Server::BUFFER_SIZE];
                    memset(message, 0, Server::BUFFER_SIZE);
                    int nbytes=0;
                    int nread=0;
                    // читаем все
                     while ((nread=recv(it->fd, message+nbytes, Server::BUFFER_SIZE-nbytes, MSG_DONTWAIT))>0){
                         nbytes+=nread;
                     }

                     // если количество байт отрицательно, значит что-то не так, надо откючтиься
                    if (nbytes < 0) {
                        ConnectionType type = _connectionTypes[it->fd]; // узнаем сообщение от сервера или клиента


                        if (type == ProxyClient) { // если от клиента, то завершаем все его соединения
                            closeConnection(it->fd);
                        }
                        else {
                            // если от сервера, находим его клиента и завершаем
                            auto sessIt = findByServer(it->fd);
                            closeConnection(sessIt->clientSock);
                        }

                    }

                    else if (nbytes>0){ // если данные есть, узнаем клиент или сервер
                        ConnectionType type = _connectionTypes[it->fd];
                        std::vector<Session>::iterator sessIt;
                        if (type == ProxyClient) {
                            sessIt = findByClient(it->fd);
                        }
                        else if (type == ProxyServer) {
                            sessIt = findByServer(it->fd);
                        }
                        // если клиент
                        if (type == ProxyClient) {
                            // если он еще не подключен к серверу, создаем сокет для сервара, подключаемся и добавляем его
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
                            }
                            // отправляем принятые данные серверу
                            int sendBytes = send(sessIt->serverSock, message, nbytes, MSG_NOSIGNAL);
                            // если они не отправились, то удаляем клиента и его сервер
                            if (sendBytes < 0) {
                                std::cerr << "client send to server error "<<sessIt->clientSock << " " <<sessIt->serverSock<<" "<<sendBytes<<" " << errno<< std::endl;
                                closeConnection(sessIt->clientSock);
                            }

                            // парсим сообщение
                            std::string parserRes = psqlManager.parseF(message, nbytes);

                            //записываем в файл
                            writeToFile(parserRes);
                            // если сообщение начинается на X, значит это сообщение об отключении клиента
                            // и мы закрываем его дескриптор
                            if (message[0] == 'X') {// terminate
                                closeConnection(sessIt->clientSock);
                            }
                        }
                        // если сервер
                        else if (type == ProxyServer && sessIt->clientSock > 0) {
                            // отправляем клиенту сообщение
                            int sendBytes = send(sessIt->clientSock, message, nbytes, MSG_NOSIGNAL);
                            //если не отправилось, значит какая-то проблема и закрываем соединение
                            if (sendBytes < 0) {
                                std::cerr << "server send to client error" << std::hex << errno<<std::dec<< std::endl;

                                closeConnection(sessIt->clientSock);
                            }
                            // парсим
                            std::string parserRes = psqlManager.parseB(message, nbytes);

                            //записываем в файл
                            writeToFile(parserRes);
                        }
                    }
                }
            }
        }
        // если в процессе работы некоторые серверы или клиенты отвалились, на их место в  _pollSet дескриптор меняется на -1,
        // чтобы не портить работу итератора в цкле, после цикла мы очищаем данный вектор
        _pollSet.erase(std::remove_if(_pollSet.begin(), _pollSet.end(), [](pollfd pfd){
            return pfd.fd == -1;
        }), _pollSet.end());


    }
}

void Server::close()
{

    // завершение работы сервера, очищаем все соединения
    for(Session s: _sessions){
        closeConnection(s.clientSock);
    }

    for(pollfd p: _pollSet)
        ::close(p.fd);

    _pollSet.clear();
    _sessions.clear();
    _connectionTypes.clear();

}


/**
 * @brief  создает сокет для подключения к серверу
 */
int Server::createServerSock()
{
    sockaddr_in addr;

    // создается неблокирующий сокет
    int sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0 );
    if (sock < 0) {
        std::cerr << "cannot create server socket";
        return sock;
    }

    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr = inet_addr(_serverIp.data());

    addr.sin_port=htons(_serverPort);

    // подключаемся к нему
    int rc = connect(sock, (sockaddr*)&addr, sizeof(addr));
    if (rc < 0 && errno != EINPROGRESS) {
        //  EINPROGRESS значит подключение еще в процессе
        std::cerr << "connect to server error "<<rc << std::endl;
        std::cout << errno <<std::endl;
        return rc;
    }

    return sock;

}


/**
 * @brief Закрываем соединение КЛИЕНТА!!!!
 */
void Server::closeConnection(int clientSock)
{
    // закрываем дескриптор
    ::close(clientSock);

    //находим итератор, по которому можем получить к какому серверу он подключен
    auto sessionIt = findByClient(clientSock);
    int serverSock = sessionIt->serverSock;
    // удляем из массивов
    _sessions.erase(sessionIt);
    _connectionTypes.erase(clientSock);

    // дескриптор в _poolSet пока ставим в -1, чтобы удалять не во время обработки pollSet-а
    findInPoll(clientSock)->fd = -1;

    // если к нему подключен сервер, то отсоединяем и его
    if (serverSock > 0) {
        ::close(serverSock);
        _connectionTypes.erase(serverSock);
        findInPoll(serverSock)->fd=-1;
    }
}


/**
 * @brief найти итератор по сессии по дескриптору клиента
 */
std::vector<Server::Session>::iterator Server::findByClient(int clientSock)
{
    return std::find_if(_sessions.begin(), _sessions.end(), [&clientSock](Session s) {
                                    return s.clientSock == clientSock;
                                });
}

/**
 * @brief найти итератор по сессии по дескриптору сервера
 */
std::vector<Server::Session>::iterator Server::findByServer(int serverSock)
{
    return std::find_if(_sessions.begin(), _sessions.end(), [&serverSock](Session s) {
                                    return s.serverSock == serverSock;
    });
}


/**
 * @brief найти в pollSet по дескриптору
 */
std::vector<pollfd>::iterator Server::findInPoll(int sock)
{
    return std::find_if(_pollSet.begin(), _pollSet.end(), [&sock](pollfd p) {
                                    return p.fd == sock;
    });
}

/**
 * @brief Запись в лог файл
 */
void Server::writeToFile(const std::string &str)
{
    std::ofstream logfile;
    logfile.open(Server::LOG_FILENAME, std::ios_base::app);
    if (logfile.is_open()) {
        logfile << str << std::endl;
        logfile.close();
    }
    else {
        std::cout << "WRITE To FILE ERROR" << std::endl;
    }
}
