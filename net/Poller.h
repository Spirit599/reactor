
#pragma once

#include <map>
#include <vector>

#include "base/Timestamp.h"
#include "net/EventLoop.h"
#include "base/noncpoyable.h"

class Channel;

class Poller : public noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop, string);
    virtual ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, ChannelList& activeChannels) = 0;

    virtual void updateChannel(Channel* channel) = 0;

    virtual void removeChannel(Channel* channel) = 0;

    virtual bool hasChannel(Channel* channel) const;

    void assertInLoopThread() const
    {
        loop_->assertInLoopThread();
    }

    string name() const { return name_; }

protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channelsMap_;

private:
    EventLoop* loop_;
    string name_;
};