
#pragma once

#include "base/Log.h"
#include "base/Timestamp.h"
#include "base/Types.h"
#include "base/noncopyable.h"
// #include "net/poller/PollerFactory.h"
#include "net/TimerId.h"

#include <thread>
#include <mutex>

class Poller;
class PollerFactory;
class Channel;
class TimerQueue;

class EventLoop : public noncopyable
{
public:

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

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

    void runInLoop(Functor cb);

    TimerId runAt(Timestamp time, TimerCallback cb);
    TimerId runAfter(double delay, TimerCallback cb);
    TimerId runEvery(double interval, TimerCallback cb);
    void cancel(TimerId timerId);


    
private:
    void abortNotInLoopThread()
    {
        LOG_FATAL("abortNotInLoopThread() threadId_ = %d but current thread id %d", threadId_, std::this_thread::get_id());
    }

    std::thread::id threadId_;
    bool looping_;
    bool quit_; //todo
    bool eventHandling_;
    bool callingPendingFunctors_;

    Timestamp pollReturnTime_;
    std::unique_ptr<PollerFactory> pollerFactory_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels_;
    Channel* curChannel_;

    std::mutex mutex_;
    
    typedef std::vector<Functor> FunctorList;
    FunctorList pengdingFunctors_;
};