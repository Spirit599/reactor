#include "net/TcpConnection.h"


void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        LOG_TRACE("%s->%s is UP", conn->localAddr().addrToIpAddr().c_str(), conn->peerAddr().addrToIpAddr().c_str());
    }
    else
    {
        LOG_TRACE("%s->%s is DOWN", conn->localAddr().addrToIpAddr().c_str(), conn->peerAddr().addrToIpAddr().c_str());
    }
}

void defaultMessageCallback(const TcpConnectionPtr& conn)
{
    LOG_TRACE("get message:%s", conn->inputBuffer().retrieveAllAsString().c_str());
}

TcpConnection::TcpConnection(EventLoop* loop, const string& name, int fd,
                    const InetAddress& localAddr, const InetAddress& peerAddr)
    :
    loop_(loop),
    name_(name),
    reading_(false),
    tcpConnectionSocket_(fd),
    tcpConnectionChannel_(loop, fd),
    localAddr_(localAddr),
    peerAddr_(peerAddr),
    state_(KConnecting),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback)
{
    tcpConnectionChannel_.setReadCallback(std::bind(&TcpConnection::handleRead, this));
    tcpConnectionChannel_.setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    tcpConnectionChannel_.setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    
    tcpConnectionSocket_.setKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    LOG_DEBUG("~TcpConnection() fd:%d", tcpConnectionSocket_.fd());
}

void TcpConnection::connectionEstablished()
{
    assert(state_ == KConnecting);
    setState(KConnected);
    
    tcpConnectionChannel_.enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectionDestroyed()
{
    if(state_ == KConnected)
    {
        setState(KDisconnected);
        tcpConnectionChannel_.disableAll();
        connectionCallback_(shared_from_this());
    }
    tcpConnectionChannel_.remove();
}


void TcpConnection::handleRead()
{
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(tcpConnectionSocket_.fd(), &savedErrno);
    if(n > 0)
    {
        messageCallback_(shared_from_this());
    }
    else if(n == 0)
    {
        handleClose();
    }
    else
    {
        //todo
    }
}


void TcpConnection::handleClose()
{
    assert(state_ == KConnected || state_ == KDisconnecting);
    setState(KDisconnected);

    tcpConnectionChannel_.disableAll();
    TcpConnectionPtr guardThis(shared_from_this());

    connectionCallback_(guardThis);
    
    closeCallback_(guardThis);
}

void TcpConnection::handleWrite()
{
    
}