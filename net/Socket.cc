
#include "net/Socket.h"
#include "net/SocketOps.h"

#include <cstring>
#include <unistd.h>

Socket::~Socket()
{
    ::close(fd_);
}

void Socket::bindAddress(const InetAddress& addr)
{
    bindOrDie(fd_, addr.getSockAddr());
}

void Socket::startListen()
{
    listenOrDie(fd_);
}

int Socket::accept(InetAddress& peerAddr)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int connfd = acceptAndDeal(fd_, &addr);
    if(connfd >= 0)
    {
        peerAddr.setSockAddr(addr);
    }
    return connfd;
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
}