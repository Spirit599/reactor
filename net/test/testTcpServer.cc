#include "net/TcpServer.h"

int main(int argc, char const *argv[])
{
    EventLoop loop;

    InetAddress addr(8888);
    TcpServer tcpServer(&loop, addr, "tcp");
    tcpServer.setThreadNum(4);
    tcpServer.start();

    loop.loop();
    return 0;
}
