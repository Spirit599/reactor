
#include "net/EventLoopThreadPool.h"

int main(int argc, char const *argv[])
{
    EventLoop loop;
    EventLoopThreadPool pools(&loop, "tcp");
    pools.setThreadNum(2);
    pools.start();
    loop.loop();
    return 0;
}
