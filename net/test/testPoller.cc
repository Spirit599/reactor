
#include <iostream>

#include "net/poller/PollerFactory.h"


int main(int argc, char const *argv[])
{
    EventLoop loop;
    PollerFactory* pollerFactory = new EpollPollerFactory();
    Poller* poller = pollerFactory->CreatePoller(&loop);
    std::cout << poller->name() << std::endl;

    return 0;
}
