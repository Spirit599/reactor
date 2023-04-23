#pragma once

#include "base/Types.h"
#include "net/TcpConnection.h"
#include "net/EventLoopThreadPool.h"
#include "net/Acceptor.h"

class TcpServer : public noncopyable
{
public:
    TcpServer(EventLoop* loop, const InetAddress& localAddr, const string& name);
    ~TcpServer();

    void newConntion(int fd, const InetAddress& addr);

    void start();
    

private:
    EventLoop* loop_;
    string name_;
    string ipPort_;

    Acceptor acceptor_;
    EventLoopThreadPool threadsPool_;

    bool started_;

    int nextConnId_;
};
