#pragma once

#include "base/copyable.h"
#include "base/Types.h"

#include <netinet/in.h>

class InetAddress : public copyable
{
public:

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

private:
    struct sockaddr_in addr_;
};