#include <iostream>
#include "net/EventLoop.h"
using namespace std;

void fun1()
{
    std::cout<< "fun1()" << std::endl;
}

int main(int argc, char const *argv[])
{
    
    EventLoop loop;

    loop.runEvery(1, std::bind(&fun1));

    loop.loop();

    return 0;
}
