#include <iostream>
#include <unistd.h>
#include "base/ThreadPool.h"


#include <atomic>
std::atomic<int> check = 0;

void fun1()
{
    check.fetch_add(1, std::memory_order_relaxed);
    std::cout << check << std::endl;
}

int main(int argc, char const *argv[])
{

    

    ThreadPool threadPool(128, "testThreadPool");
    threadPool.start(4);

    
    for(int i = 0; i < 10000; ++i)
        threadPool.put(fun1);

    sleep(10);
    return 0;
}
