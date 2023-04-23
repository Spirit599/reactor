
#include "net/SocketOps.h"
#include "base/Log.h"

int createNonblockingOrDie(sa_family_t fa)
{
    int fd = ::socket(fa, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
    if(fd < 0)
    {
        LOG_FATAL("createNonblockingOrDie");
    }
    return fd;
}

void bindOrDie(int fd, const struct sockaddr* addr)
{
    int ret = ::bind(fd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if(ret < 0)
    {
        LOG_FATAL("bindOrDie");
    }
}

void listenOrDie(int fd)
{
    int ret = ::listen(fd, SOMAXCONN);
    if(ret < 0)
    {
        LOG_FATAL("listenOrDie");
    }
}

int acceptAndDeal(int fd, struct sockaddr_in* addr)
{
    socklen_t addrLen = static_cast<socklen_t>(sizeof(sockaddr_in));

    int connfd = ::accept4(fd, reinterpret_cast<struct sockaddr*>(addr), &addrLen, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if(connfd < 0)
    {
        int savedErrno = errno;
        LOG_ERROR("acceptAndDeal %d", savedErrno);
    }

    return connfd;
}

void createAddrFromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = htobe16(port);
    if(::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        LOG_FATAL("createAddrFromIpPort");
    }
}

void toIp(char* buf, size_t size, const struct sockaddr_in* addr)
{
    ::inet_ntop(AF_INET, &addr->sin_addr, buf, static_cast<socklen_t>(size));
}

void toIpPort(char* buf, size_t size, const struct sockaddr_in* addr)
{
    toIp(buf, size, addr);
    size_t end = strlen(buf);
    snprintf(buf + end, size - end, ":%u", be16toh(addr->sin_port));
}