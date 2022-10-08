#include "server.h"
#include <iostream>
int main (int argc, char *argv[])
{
    int proxyPort = 8080;
    std::string psqlIp = "127.0.0.1";
    int psqlPort = 5432;
    Server server{proxyPort, psqlIp, psqlPort};
    if (server.start())
        server.run();
    std::cout <<"Server failed";
    server.close();
    return 0;
}

