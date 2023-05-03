#pragma once

#include "base/noncopyable.h"
#include "base/Types.h"

#include <thread>
#include <mutex>
#include <condition_variable>

class EventLoop;

class EventLoopThread : public noncopyable
{
public:
    EventLoopThread(const string& name = string());
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();
    
    EventLoop* loop_;
    bool exiting_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
};