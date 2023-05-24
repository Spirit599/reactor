#pragma once

#include <vector>
#include <atomic>
#include <iostream>
#include <assert.h>

template<typename T>
class BounderBlockingQueue
{
public:
    BounderBlockingQueue(size_t capacity)
    :
    capacity_(capacity),
    arr_(capacity),
    startIdx_(0),
    endIdx_(0)
    {
        
        assert((capacity & (capacity - 1)) == 0);
    }

    ~BounderBlockingQueue() = default;

    bool put(T data)
    {
        // cout<<k<<endl;
        size_t endIdx = 0;
        Elem* e = NULL;
        do
        {
            endIdx = endIdx_.load(std::memory_order_relaxed);
            if(endIdx >= startIdx_.load(std::memory_order_relaxed) + capacity_) // todo
                return false;
            size_t idx = endIdx & (capacity_ - 1);
            e = &arr_[idx];
            if(e->full_.load(std::memory_order_relaxed))
                return false;
            
        }while(!endIdx_.compare_exchange_weak(endIdx, endIdx + 1, std::memory_order_release, std::memory_order_relaxed));
        
        e->data_ = data;
        e->full_.store(true, std::memory_order_release);
        return true;
    }

    bool take(T& data)
    {
        size_t startIdx = 0;
        Elem* e = NULL;
        do
        {

            startIdx = startIdx_.load(std::memory_order_relaxed);
            if(startIdx >= endIdx_.load(std::memory_order_relaxed))
                return false;
            size_t idx = startIdx & (capacity_ - 1);
            e = &arr_[idx];
            if(!e->full_.load(std::memory_order_relaxed))
                return false;

        } while (!startIdx_.compare_exchange_weak(startIdx, startIdx + 1, std::memory_order_release, std::memory_order_relaxed));
        
        data = e->data_;
        e->full_.store(false, std::memory_order_release);
        return true;
    }

private:
    struct Elem
    {
        std::atomic<bool> full_;
        T data_;
    };
    std::vector<Elem> arr_;
    std::atomic<size_t> startIdx_;
    std::atomic<size_t> endIdx_;
    size_t capacity_;
};