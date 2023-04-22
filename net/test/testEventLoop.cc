
#include <iostream>

#include "net/EventLoop.h"
#include "net/poller/PollerFactory.h"
#include <chrono>



int main(int argc, char const *argv[])
{
    
    EventLoop loop;

    

    loop.loop();



    return 0;
    
}
