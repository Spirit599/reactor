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

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
{
    string message(conn->inputBuffer().retrieveAllAsString());
    LOG_TRACE("get message:%s", message.c_str());
    conn->sendMessage(message.c_str(), message.size());
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
    tcpConnectionChannel_.setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
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

void TcpConnection::sendMessage(Buffer* buf)
{
    if(loop_->isInLoopThread())
    {
        sendMessage(buf->peek(), buf->readableBytes());
        buf->retrieveAll();
    }
}

void TcpConnection::sendMessage(const char* data, size_t len)
{
    ssize_t nwrote = 0;
    size_t remaining = len;

    //没在写 并且写缓冲区没数据 直接发到内核
    if(!tcpConnectionChannel_.isWriting() && outputBuffer_.readableBytes() == 0)
    {
        nwrote = ::write(tcpConnectionChannel_.fd(), data, len);
        if(nwrote >= 0)
        {
            remaining -= nwrote;
            if(remaining == 0)
            {
                //todo
                ;
            }
        }
        else
        {
            nwrote = 0;
            LOG_ERROR("TcpConnection::send()");
        }
    }

    if(remaining > 0)
    {
        outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
        if(!tcpConnectionChannel_.isWriting())
        {
            tcpConnectionChannel_.enablewriting();
        }
    }
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(tcpConnectionSocket_.fd(), &savedErrno);
    if(n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
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
    if(tcpConnectionChannel_.isWriting())
    {
        ssize_t n = ::write(tcpConnectionChannel_.fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if(n > 0)
        {
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0)
            {
                tcpConnectionChannel_.disablewriting();
                //todo
            }
        }
    }
}