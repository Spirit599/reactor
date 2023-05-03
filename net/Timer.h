#pragma once

#include "base/Timestamp.h"
#include "base/noncopyable.h"
#include "base/Types.h"

#include <atomic>

class Timer : public noncopyable
{
public:
    Timer(TimerCallback cb, Timestamp when, double interval)
        :
        callback_(std::move(cb)),
        expiration_(when),
        interval_(interval),
        repeat_(interval > 0.0),
        sequence_(s_numCreated_.fetch_add(1))
    {}

    void run() const
    { callback_(); }

    Timestamp expiration() const
    { return expiration_; }

    bool repeat() const
    { return repeat_; }

    int64_t sequence()
    { return sequence_; }

    void restart(Timestamp now);

private:
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;
    
    static std::atomic<int64_t> s_numCreated_;
};