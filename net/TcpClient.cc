#include "net/TcpClient.h"

#include "base/Log.h"
#include "net/Connector.h"
#include "net/EventLoop.h"
#include "SocketOps.h"

#include <stdio.h>


void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
{
    loop->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector)
{

}


TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg)
    :
    loop_(loop),
    connector_(new Connector(loop, serverAddr)),
    name_(nameArg),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback),
    connect_(true),
    nextConnId_(1)
{
    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, _1));
}

TcpClient::~TcpClient()
{
    TcpConnectionPtr conn;
    bool unique = false;

    {
        std::unique_lock<std::mutex> ulk(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }

    if(conn)
    {
        CloseCallback cb = std::bind(&::removeConnection, loop_, _1);
        loop_->runInLoop(std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if(unique)
        {
            conn->handleClose();
        }
    }
    else
    {
        connector_->stop();
        // loop_->runAfter(1, std::bind(&))
    }
}

void TcpClient::connect()
{

    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect()
{
    connect_ = false;

    {
        std::unique_lock<std::mutex> ulk(mutex_);
        if(connection_)
        {
            //shutdown?
            connection_->shutdown();
        }
    }
}

void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int fd)
{
    InetAddress peerAddr(getPeerAddr(fd));
    char buf[32];
    snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.addrToIpAddr().c_str(), nextConnId_);
    ++nextConnId_;

    string name = name_ + buf;

    LOG_INFO("newConnection %s", name.c_str());

    InetAddress localAddr(getLocalAddr(fd));

    TcpConnectionPtr conn(new TcpConnection(loop_, name, fd, localAddr, peerAddr));
    
    

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpClient::removeConnection, this, _1));

    {
        std::unique_lock<std::mutex> ulk(mutex_);
        connection_ = conn;
    }
    conn->connectionEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    {
        std::unique_lock<std::mutex> ulk(mutex_);
        connection_.reset();
        LOG_INFO("connection_.reset();");
    }
    loop_->queueInLoop(std::bind(&TcpConnection::connectionDestroyed, conn));
}