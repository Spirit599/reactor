#include "net/TcpConnection.h"


TcpConnection::TcpConnection(EventLoop* loop, const string& name, int fd,
                    const InetAddress& localAddr, const InetAddress& peerAddr)
    :
    loop_(loop),
    name_(name),
    reading_(false),
    tcpConnectionSocket_(fd),
    tcpConnectionChannel_(loop, fd),
    localAddr_(localAddr),
    peerAddr_(peerAddr)
{
    tcpConnectionSocket_.setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG("~TcpConnection() fd:%d", tcpConnectionSocket_.fd());
}