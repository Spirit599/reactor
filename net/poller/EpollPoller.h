#pragma once

#include "net/Poller.h"


struct epoll_event;

class EpollPoller : public Poller
{
public:

    EpollPoller(EventLoop* loop);
    ~EpollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;

    void updateChannel(Channel* channel) override;

    void removeChannel(Channel* channel) override;

private:
    static const int kInitEventSize = 16;


    void fillActiveChannels(int numEvents, ChannelList* activeChannel) const;

    void update(int op, Channel* channel);

    typedef std::vector<struct epoll_event> EventList;

    int epfd_;
    EventList events_;
};