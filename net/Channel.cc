
#include "net/Channel.h"
#include "poll.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop* loop, int fd)
    :
    fd_(fd),
    events_(0),
    revents_(0),
    status_(-1), //kNew
    loop_(loop),
    addedToLoop(false),
    eventHandling_(false)
{}

Channel::~Channel()
{

}

void Channel::update()
{
    addedToLoop = true;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    addedToLoop = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp time)
{
    eventHandling_ = true;

    if((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if(closeCallback_)
            closeCallback_();
    }

    if(revents_ & (POLLERR | POLLNVAL))
    {
        if(errorCallback_)
            errorCallback_();
    }

    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if(readCallback_)
            readCallback_(time);
    }

    if(revents_ & (POLLOUT))
    {
        if(writeCallback_)
            writeCallback_();
    }

    eventHandling_ = false;
}