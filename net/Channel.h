#pragma once

#include "net/EventLoop.h"

class Channel
{
public:

    Channel(EventLoop* loop, int fd);
    ~Channel();

    int fd() const { return fd_; }
    int events() const { return events_; }
    void setRevents(int revent) { revents_ = revent; }
    int status() const { return status_; }
    void setStatus(int s) { status_ = s; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    void remove();
    void handleEvent();

private:

    void update();

    int fd_;
    int events_;
    int revents_;
    int status_;
    
    EventLoop* loop_;

    bool addedToLoop;
    bool eventHandling_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;


    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
};