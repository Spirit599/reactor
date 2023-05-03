#pragma once

#include "base/noncopyable.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include <vector>


class EventLoopThreadPool : public noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseLoop, const string& name);
    ~EventLoopThreadPool();

    void start();

    EventLoop* getNextLoop();

    void setThreadNum(int numThread)
    { numThread_ = numThread; }
    
    bool started() const
    { return started_; }

    const string& name() const
    { return name_; }

private:
    EventLoop* baseLoop_;
    string name_;
    bool started_;
    int numThread_;
    int curLoopIdx_;

    std::vector<std::unique_ptr<EventLoopThread>> threadsPool_;
    std::vector<EventLoop*> loops_;

};