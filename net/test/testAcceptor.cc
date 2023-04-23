#include "net/Acceptor.h"
#include "net/EventLoop.h"

int main(int argc, char const *argv[])
{
    
    EventLoop loop;

    InetAddress addr(8888);

    Acceptor acceptor(&loop, addr);
    acceptor.startListen();

    loop.loop();

    return 0;
}
