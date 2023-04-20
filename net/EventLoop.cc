
#include "EventLoop.h"

EventLoop::EventLoop()
    :
    threadId_(std::this_thread::get_id())
{

}