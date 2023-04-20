#pragma once

#include "net/Poller.h"
#include "net/poller/EpollPoller.h"

class PollerFactory
{
public:
    virtual Poller* CreatePoller(EventLoop* loop) = 0;
    virtual ~PollerFactory() = default;
};



class EpollPollerFactory : public PollerFactory
{
    Poller* CreatePoller(EventLoop* loop)
    {
        return new EpollPoller(loop);
    }
    ~EpollPollerFactory() = default;
};