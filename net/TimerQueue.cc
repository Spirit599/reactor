#include "net/TimerQueue.h"

#include "base/Log.h"
#include "net/EventLoop.h"
#include "net/Timer.h"
#include "net/TimerId.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <algorithm>

int createTimerfd()
{
    int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(fd < 0)
    {
        LOG_FATAL("createTimerfd()");
    }
    return fd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmuch;
    ssize_t n = ::read(timerfd, &howmuch, sizeof(howmuch));
    if(n != sizeof(howmuch))
    {
        LOG_ERROR("readTimerfd:reads %d instead of 8", n);
    }
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue, 0, sizeof(newValue));
    memset(&oldValue, 0, sizeof(oldValue));
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if(ret < 0)
    {
        LOG_FATAL("resetTimerfd()");
    }
}


TimerQueue::TimerQueue(EventLoop* loop)
    :
    loop_(loop),
    timerfd_(createTimerfd()),
    timerChannel_(loop, timerfd_),
    callingExpiredTimers_(false)
{
    timerChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
    timerChannel_.disableAll();
    timerChannel_.remove();
    for(const Entry& timer : timers_)
    {
        delete timer.second;
    }
}

TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval)
{
    Timer* timer = new Timer(std::move(cb), when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    
    return TimerId(timer, timer->sequence());
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();
    bool firstChanged = insert(timer);

    if(firstChanged)
    {
        resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());

    ActiveTimer rmTimer(timerId.timer_, timerId.sequence_);
    auto it = activeTimers_.find(rmTimer);
    if(it != activeTimers_.end())
    {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1); (void)n;
        delete it->first;
        activeTimers_.erase(it);
    }
    else if(callingExpiredTimers_)
    {
        cancelingTimers_.insert(rmTimer);
    }

    assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::handleRead()
{
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();

    for(const Entry& timer : expired)
    {
        timer.second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    std::vector<TimerQueue::Entry> expired;
    Entry tmp(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto last = timers_.lower_bound(tmp);
    std::copy(timers_.begin(), last, back_inserter(expired));
    timers_.erase(timers_.begin(), last);

    for(const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequence());
        size_t n = activeTimers_.erase(timer);
        assert(n == 1); (void)n;
    }

    return expired;
}

void TimerQueue::reset(const std::vector<TimerQueue::Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;

    for(const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequence());
        if(it.second->repeat() && cancelingTimers_.find(timer) == cancelingTimers_.end())
        {
            it.second->restart(now);
            insert(it.second);
        }
        else
        {
            delete it.second;
        }
    }

    if(!timers_.empty())
    {
        nextExpire = timers_.begin()->second->expiration();
    }

    if(nextExpire.isVaild())
    {
        resetTimerfd(timerfd_, nextExpire);
    }
}

bool TimerQueue::insert(Timer* timer)
{
    bool firstChanged = false;
    Timestamp when = timer->expiration();
    auto itFirst = timers_.begin();
    if(itFirst == timers_.end() || when < itFirst->first)
    {
        firstChanged = true;
    }

    timers_.insert(Entry(when, timer));
    activeTimers_.insert(ActiveTimer(timer, timer->sequence()));

    return firstChanged;
}