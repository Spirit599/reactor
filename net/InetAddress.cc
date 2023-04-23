
#include "net/InetAddress.h"





InetAddress::InetAddress(const char* ip, uint16_t port)
{
    memset(&addr_, 0, sizeof(addr_));
    createAddrFromIpPort(ip, port, &addr_);
}

InetAddress::InetAddress(uint16_t port)
{
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = INADDR_ANY;        //ALL
    addr_.sin_addr.s_addr = htobe32(ip);
    addr_.sin_port = htobe16(port);
}