#include <iostream>
#include "net/EventLoop.h"
#include "net/TcpClient.h"

int main(int argc, char const *argv[])
{
    
    EventLoop loop;

    InetAddress serverAddr(8888);
    TcpClient c1(&loop, serverAddr, "c1");
    c1.connect();

    loop.loop();

    return 0;
}
