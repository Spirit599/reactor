
#include "net/Poller.h"
#include "net/Channel.h"

Poller::Poller(EventLoop* loop, string name = "poller")
    :
    loop_(loop),
    name_(name)
{

}


bool Poller::hasChannel(Channel* channel) const
{
    assertInLoopThread();
    auto it = channelsMap_.find(channel->fd());
    return it != channelsMap_.end() && it->second == channel;
}