#include "net/TcpServer.h"
// #include ""

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& name)
    :
    loop_(loop),
    name_(name),
    ipPort_(listenAddr.addrToIpAddr()),
    acceptor_(loop, listenAddr),
    threadsPool_(loop, name),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback)
{
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConntion, this, _1, _2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
    started_ = true;
    threadsPool_.start();
    acceptor_.startListen();
}

void TcpServer::setThreadNum(int numThread)
{
    threadsPool_.setThreadNum(numThread);
}

void TcpServer::newConntion(int connfd, const InetAddress& peerAddr)
{
    LOG_TRACE("connfd:%d ip:%s", connfd, peerAddr.addrToIpAddr().c_str());
    EventLoop* loop = threadsPool_.getNextLoop();
    string connName(name_ + "#" + ipPort_ + "#" + std::to_string(nextConnId_));
    InetAddress localAddr(getLocalAddr(connfd));

    TcpConnectionPtr conn(new TcpConnection(loop,
                                            connName,
                                            connfd,
                                            localAddr,
                                            peerAddr));
    
    connections_[connName] = conn;


    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConntion, this, _1));


    conn->connectionEstablished(); //todo

}

void TcpServer::removeConntion(const TcpConnectionPtr& conn)
{
    connections_.erase(conn->name());
    conn->connectionDestroyed(); ////todo
}

