
#include "net/EventLoop.h"
#include "net/poller/PollerFactory.h"
#include "net/Channel.h"


__thread EventLoop* t_LoopInThisThread = nullptr;
const int kPollTimeMs = 10000;

EventLoop::EventLoop()
    :
    threadId_(std::this_thread::get_id()),
    looping_(false),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    pollerFactory_(new EpollPollerFactory()),
    poller_(pollerFactory_->CreatePoller(this)),
    curChannel_(nullptr)
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