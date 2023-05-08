
#include "net/EventLoop.h"
#include "net/poller/PollerFactory.h"
#include "net/Channel.h"
#include "net/TimerQueue.h"

#include "sys/eventfd.h"

__thread EventLoop* t_LoopInThisThread = nullptr;
const int kPollTimeMs = 10000;

int createEventfd()
{
    int evfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evfd < 0)
    {
        LOG_FATAL("createEventfd");
    }
    return evfd;
}


EventLoop::EventLoop()
    :
    threadId_(std::this_thread::get_id()),
    looping_(false),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    pollerFactory_(new EpollPollerFactory()),
    poller_(pollerFactory_->CreatePoller(this)),
    timerQueue_(new TimerQueue(this)),
    curChannel_(nullptr),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_))
{
    LOG_DEBUG("create threadId_:%llu", threadId_);
    if(t_LoopInThisThread)
    {
        LOG_FATAL("other EventLoop has create");
    }
    else
    {
        t_LoopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    LOG_DEBUG("destructs threadId_:%llu", threadId_);
    t_LoopInThisThread = nullptr;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();

    looping_ = true;
    quit_ = false;

    LOG_TRACE("eventloop threadId_:%llu start loop", threadId_);

    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, activeChannels_);

        eventHandling_ = true;
        for(Channel* channel : activeChannels_)
        {
            curChannel_ = channel;
            channel->handleEvent();
        }
        curChannel_ = nullptr;
        eventHandling_ = false;
        doPendingFunctors();
    }

    LOG_TRACE("eventloop threadId_:%llu end loop", threadId_);
    looping_ = false;
}

void EventLoop::quit()
{
    LOG_TRACE("EventLoop::quit() threadId_:%llu", threadId_);
    quit_ = true;
}


void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> funs;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lockGurad(mutex_);
        funs.swap(pengdingFunctors_);
    }

    for(const Functor& f : funs)
    {
        f();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread())
    {
        cb();
    }
    else
    {
        {
            std::unique_lock<std::mutex> lockGurad(mutex_);
            pengdingFunctors_.emplace_back(std::move(cb));
        }
        wakeup();
    }
}

TimerId EventLoop::runAt(Timestamp time, TimerCallback cb)
{
    return timerQueue_->addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(std::move(cb), time, interval);
}

void EventLoop::cancel(TimerId timerId)
{
    return timerQueue_->cancel(timerId);
}