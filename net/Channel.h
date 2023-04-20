#pragma once

class Channel
{
public:
    int fd() const { return fd_; }
    int events() const { return events_; }
    void setRevents(int revent) { revents_ = revent; }
    int status() const { return status_; }
    void setStatus(int s) { status_ = s; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }
private:
    int fd_;
    int events_;
    int revents_;
    int status_;

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
};