
#include "net/poller/EpollPoller.h"

#include "net/Channel.h"
#include "base/Log.h"

#include <sys/epoll.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;


EpollPoller::EpollPoller(EventLoop* loop)
    :
    Poller(loop, "EpollPoller"),
    epfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventSize)
{
    if(epfd_ < 0)
    {
        LOG_FATAL("EpollPoller: epfd_ < 0 ");
    }
}

EpollPoller::~EpollPoller()
{
    LOG_TRACE("~EpollPoller()");
    ::close(epfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList& activeChannels)
{
    LOG_TRACE("fd total count %d", channelsMap_.size());
    int numEvents = ::epoll_wait(epfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);

    Timestamp now(Timestamp::now());
    int saveErrno = errno;
    if(numEvents > 0)
    {
        LOG_TRACE("%d events happened", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if(static_cast<size_t>(numEvents) == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if(numEvents == 0)
    {
        LOG_TRACE("nothing happened");
    }
    else
    {
        if(saveErrno != EINTR) // Error Interrupted
        {
            errno = saveErrno;
            LOG_ERROR("saveErrno != EINTR");
        }
    }
    return now;
}


void EpollPoller::fillActiveChannels(int numEvents, ChannelList& activeChannels) const
{
    for(int i = 0; i < numEvents; ++i)
    {
        Channel* channel =  static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels.emplace_back(channel);
    }
}

void EpollPoller::update(int op, Channel* channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    LOG_TRACE("op:%d fd:%d events:%d", op, fd, channel->events());

    if(::epoll_ctl(epfd_, op, fd, &event) < 0)
    {
        if(op == EPOLL_CTL_DEL)
        {
            LOG_ERROR("fd:%d epoll_ctl op:%d", fd, op);
        }
        else
        {
            LOG_FATAL("fd:%d epoll_ctl op:%d", fd, op);
        }
    }
}

void EpollPoller::updateChannel(Channel* channel)
{
    assertInLoopThread();
    const int status = channel->status();
    int fd = channel->fd();
    LOG_TRACE("fd:%d events:%d status:%d", fd, channel->events(), channel->status());
    if(status == kNew || status == kDeleted)
    {
        
        if(status == kNew)
        {
            channelsMap_[fd] = channel;
        }
        channel->setStatus(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        if(channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setStatus(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel* channel)
{
    assertInLoopThread();
    int fd = channel->fd();
    int status = channel->status();
    LOG_TRACE("removeChannel fd:%d", fd);
    // assert(channelsMap_.find(fd) != channelsMap_.end());
    auto it = channelsMap_.find(fd);
    assert(it != channelsMap_.end());
    channelsMap_.erase(it);

    if(status == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setStatus(kNew);
}