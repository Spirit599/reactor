
#include "base/ThreadPool.h"

ThreadPool::ThreadPool(int maxQueueSize, const string& nameArg)
    :
    maxQueueSize_(maxQueueSize_),
    name_(nameArg),
    running_(false),
    taskQueue_(maxQueueSize)
{}

ThreadPool::~ThreadPool()
{
    if(running_)
    {
        stop();
    }
}

void ThreadPool::start(int numThreads)
{
    running_ = true;
    threads_.reserve(numThreads);
    for(int i = 0; i < numThreads; ++i)
    {
        threads_.emplace_back(new std::thread(std::bind(&ThreadPool::runInThread, this)));
    }
}

void ThreadPool::stop()
{
    running_ = false;
    for(auto& thr : threads_)
    {
        thr->join();
    }
}

void ThreadPool::put(Task task)
{
    while(!taskQueue_.put(task))
        ;
}

void ThreadPool::take(Task& task)
{
    while(!taskQueue_.take(task) && running_)
        ;
}

void ThreadPool::runInThread()
{
    while(running_)
    {
        Task task;
        take(task);
        if(task)
            task();
    }
}