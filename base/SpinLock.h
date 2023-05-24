#pragma once


#include <atomic>

class SpinMutex
{
private:
    std::atomic<bool> flag;
public:
    SpinMutex()
        :
        flag(false)
    {}

    void lock()
    {
        for(;;)
        {
            if(!flag.exchange(true, std::memory_order_acquire))
                break;
            while(flag.load(std::memory_order_relaxed))
            {
                ;
            }
        }
    }

    void unlock()
    {
        flag.store(false, std::memory_order_release);
    }
};

class SpinLock
{
public:
	SpinLock(SpinMutex& mtx) : mutex_(mtx) { mutex_.lock(); }
	SpinLock(const SpinLock&) = delete;
	SpinLock& operator=(const SpinLock&) = delete;
	~SpinLock() { mutex_.unlock(); }

private:
	SpinMutex& mutex_;
};