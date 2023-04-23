
#include "net/Acceptor.h"
#include "net/EventLoop.h"
#include "net/SocketOps.h"

#include <fcntl.h>

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr)
    :
    loop_(loop),
    acceptSocket_(createNonblockingOrDie(addr.family())),
    acceptChannel_(loop, acceptSocket_.fd()),
    listening_(false),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    acceptSocket_.setKeepAlive(true);
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(addr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
    ::close(idleFd_);
}

void Acceptor::startListen()
{
    listening_ = true;
    acceptSocket_.startListen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(peerAddr);
    if(connfd >= 0)
    {
        LOG_TRACE("connfd:%d ip:%s", connfd, peerAddr.addrToIpAddr().c_str());
        ::close(connfd);
    }
    else
    {
        if(errno == EMFILE)
        {
            ::close(idleFd_);
            idleFd_ = ::accept(acceptSocket_.fd(), nullptr, nullptr);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}