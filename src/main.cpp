#include "server.h"

int main (int argc, char *argv[])
{
    Server server{8888, "127.0.0.1", 12349};
    if (server.start())
        server.run();
    server.close();
    return 0;
}

