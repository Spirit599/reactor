#pragma once

#include "base/copyable.h"
#include "base/Types.h"

class Timer;

class TimerId : public copyable
{
public:
    TimerId()
        :
        timer_(nullptr),
        sequence_(0)
    {}

    TimerId(Timer* timer, int64_t seq)
        :
        timer_(timer),
        sequence_(seq)
    {}

    friend class TimerQueue;

private:
    Timer* timer_;
    int64_t sequence_;
};