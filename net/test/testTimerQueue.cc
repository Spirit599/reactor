#include <iostream>
#include "net/EventLoop.h"
using namespace std;

void fun2()
{
    std::cout<< "fun2()" << std::endl;
}

void fun1(EventLoop* loop)
{
    // std::cout<< "fun1()" << std::endl;
    loop->runEvery(1, std::bind(&fun2));
}



int main(int argc, char const *argv[])
{
    
    EventLoop loop;

    // loop.runEvery(1, std::bind(&fun1));

    std::thread thr(fun1, &loop);

    loop.loop();

    return 0;
}
