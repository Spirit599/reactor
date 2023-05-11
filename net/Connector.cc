#include "net/Connector.h"

#include "net/EventLoop.h"
#include "base/Log.h"
#include "net/Channel.h"
#include "net/SocketOps.h"

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
    :
    loop_(loop),
    serverAddr_(serverAddr),
    connect_(false),
    state_(KDisconnected)
{}

Connector::~Connector()
{
    assert(!channel_);
}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
    assert(state_ == KDisconnected);
    if(connect_)
    {
        LOG_INFO("44444444444444444444444");
        connect();
    }
    else
    {
        LOG_DEBUG("do not connect");
    }
}

void Connector::stop()
{
    connect_ = false;
    loop_->queueInLoop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop()
{
    if(state_ == KConnecting)
    {
        ;
    }
}

void Connector::connect()
{
    int fd = createNonblockingOrDie(serverAddr_.family());
    int ret = doConnection(fd, serverAddr_.getSockAddr());

    LOG_INFO("connect() %s", serverAddr_.addrToIpAddr().c_str());

    if(ret == 0)
    {
        LOG_INFO("55555555555555555555");
        connecting(fd);
    }
    else
    {
        LOG_ERROR("connect() %d", ret);
    }
}

void Connector::connecting(int fd)
{
    setState(KConnecting);
    channel_.reset(new Channel(loop_, fd));
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));

    channel_->enablewriting();
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int fd = channel_->fd();

    loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
    return fd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

void Connector::handleWrite()
{
    if(state_ == KConnecting)
    {
        int fd = removeAndResetChannel();
        setState(KConnected);
        if(connect_)
        {
            LOG_INFO("333333333333333333");
            newConnectionCallback_(fd);
        }
        else
        {
            ::close(fd);
        }
    }
}