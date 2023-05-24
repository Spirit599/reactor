#pragma once

#include "base/BounderBlockingQueue.h"
#include "base/noncopyable.h"
#include "base/Types.h"

#include <functional>
#include <memory>
#include <thread>

class ThreadPool : public noncopyable
{
public:
    typedef std::function<void()> Task;

    ThreadPool(int queueSize, const string& nameArg);
    ~ThreadPool();

    void start(int numThreads);
    void stop();
    void put(Task task);

private:

    void runInThread();
    void take(Task& task);

    int maxQueueSize_;
    string name_;
    bool running_;
    BounderBlockingQueue<Task> taskQueue_;
    std::vector<std::unique_ptr<std::thread>> threads_;
};