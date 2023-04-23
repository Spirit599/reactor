
#include "net/EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& name)
    :
    baseLoop_(baseLoop),
    name_(name),
    started_(false),
    numThread_(0),
    curLoopIdx_(0)
{}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start()
{
    started_ = true;
    string poolName(name_);
    for(int i = 0; i < numThread_; ++i)
    {
        EventLoopThread* th = new EventLoopThread(name_ + std::to_string(i));
        threadsPool_.emplace_back(std::unique_ptr<EventLoopThread>(th));
        loops_.emplace_back(th->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = baseLoop_;
    if(!loops_.empty())
    {
        loop = loops_[curLoopIdx_];
        ++curLoopIdx_;
        if(static_cast<size_t>(curLoopIdx_) == loops_.size())
        {
            curLoopIdx_ = 0;
        }
    }
    return loop;
}