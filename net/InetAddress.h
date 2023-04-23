#pragma once

#include "base/copyable.h"
#include "base/Types.h"
#include "net/SocketOps.h"

#include <netinet/in.h>
#include <cstring>

class InetAddress : public copyable
{
public:

    InetAddress()
    {
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
    }

    InetAddress(const char* ip, uint16_t port);
    InetAddress(uint16_t port);

    explicit InetAddress(const struct sockaddr_in& addr)
        :
        addr_(addr)
    {}

    sa_family_t family() const
    { return addr_.sin_family; }

    const struct sockaddr* getSockAddr() const
    {
        return reinterpret_cast<const struct sockaddr*>(&addr_);
    }

    const struct sockaddr_in* getSockAddrIn() const
    {
        return &addr_;
    }

    void setSockAddr(const struct sockaddr_in& addr)
    {
        addr_ = addr;
    }

    string addrToIpAddr() const
    {
        char buf[64];
        toIpPort(buf, sizeof(buf), getSockAddrIn());
        return buf;
    }

private:
    struct sockaddr_in addr_;
};