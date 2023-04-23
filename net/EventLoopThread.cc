
#include "net/EventLoopThread.h"
#include "net/EventLoop.h"

EventLoopThread::EventLoopThread(const string& name)
    :
    loop_(nullptr),
    exiting_(false),
    thread_(&EventLoopThread::threadFunc, this)
{}


EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_ != nullptr)
    {
        loop_->quit();
    }
    thread_.join();
}

EventLoop* EventLoopThread::startLoop()
{
    EventLoop* loop = nullptr;

    {
        std::unique_lock<std::mutex> uniqueLock(mutex_);
        while(loop_ == nullptr)
        {
            cv_.wait(uniqueLock);
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    {
        std::unique_lock<std::mutex> uniqueLock(mutex_);
        loop_ = &loop;
        cv_.notify_all();
    }
    loop.loop();
}