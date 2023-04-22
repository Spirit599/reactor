#include <iostream>
#include "net/SockerOps.h"
#include "net/InetAddress.h"
#include "net/EventLoop.h"
using namespace std;

int main(int argc, char const *argv[])
{
    
    cout << sizeof(struct sockaddr) << endl;
    cout << sizeof(struct sockaddr_in) << endl;
    cout << sizeof(struct sockaddr_in6) << endl;

    // int fd = createNonblockingOrDie()
    EventLoop loop;

    InetAddress addr(8888);
    int fd = createNonblockingOrDie(addr.family());
    bindOrDie(fd, addr.getSockAddr());
    listenOrDie(fd);

    loop.loop();

    return 0;
}
