#include "net/TcpServer.h"
// #include ""

TcpServer::TcpServer(EventLoop* loop, const InetAddress& localAddr, const string& name)
    :
    loop_(loop),
    name_(name),
    ipPort_(localAddr.addrToIpAddr()),
    acceptor_(loop, localAddr),
    threadsPool_(loop, name)
{
    acceptor_.setNewConntionCallback(std::bind(&TcpServer::newConntion, this, _1, _2));
}

TcpServer::~TcpServer()
{

}


void TcpServer::newConntion(int connfd, const InetAddress& peerAddr)
{
    LOG_TRACE("connfd:%d ip:%s", connfd, peerAddr.addrToIpAddr().c_str());
}

void TcpServer::start()
{
    started_ = true;
    threadsPool_.start();
    acceptor_.startListen();
}