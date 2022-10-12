#include "server.h"
#include <iostream>
int main (int argc, char *argv[])
{

    // порт ставим 8080
    int proxyPort = 8080;

    // postgresql тоже на локальной машине
    std::string psqlIp = "127.0.0.1";
    int psqlPort = 5432;

    Server server{proxyPort, psqlIp, psqlPort};

    if (server.start()) // если сервер стартовал
        server.run(); // то запускаем основной цикл
    std::cout <<"Server failed"; // если основной цикл прервался, значит что-то пошло не так
    server.close(); // закрываем соединения
    return 0;
}

