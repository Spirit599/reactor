
#pragma once

#include "base/Log.h"

#include <thread>

class EventLoop
{
public:

    EventLoop();

    void assertInLoopThread()
    {
        if(!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const
    {
        return threadId_ == std::this_thread::get_id();
    }
    
private:
    void abortNotInLoopThread()
    {
        LOG_FATAL("abortNotInLoopThread() threadId_ = %d but current thread id %d", threadId_, std::this_thread::get_id());
    }

    std::thread::id threadId_;
};