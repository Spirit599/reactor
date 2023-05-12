#pragma once

#include "base/noncopyable.h"
#include "net/InetAddress.h"

class Socket : public noncopyable
{
public:
    explicit Socket(int fd)
        :
        fd_(fd)
    {}

    ~Socket();

    int fd() const
    { return fd_; }

    void bindAddress(const InetAddress& addr);

    void startListen();

    int accept(InetAddress& peerAddr);

    void setReuseAddr(bool on);

    void setKeepAlive(bool on);

    void shutdown()
    { ::shutdown(fd_, SHUT_WR); }

private:
    const int fd_;
};