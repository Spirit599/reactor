
#include <iostream>

#include "net/EventLoop.h"
#include "net/poller/PollerFactory.h"

int main(int argc, char const *argv[])
{
    
    EventLoop loop;
    loop.assertInLoopThread();
    return 0;
}
