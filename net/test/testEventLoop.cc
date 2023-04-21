
#include <iostream>

#include "net/EventLoop.h"
#include "net/poller/PollerFactory.h"
#include <chrono>

void func(EventLoop* loop)
{
    std::this_thread::sleep_for(std::chrono::seconds(21));
    loop->quit();
}


int main(int argc, char const *argv[])
{
    
    EventLoop loop;


    std::thread thr(func, &loop);
    

    loop.loop();

    thr.join();


    return 0;
    
}
