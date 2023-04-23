#pragma once

#include "base/noncpoyable.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
#include "net/Channel.h"

class TcpConnection : public noncopyable
{
public:
    TcpConnection(EventLoop* loop, const string& name, int fd,
                    const InetAddress& localAddr, const InetAddress& peerAddr);
    
    ~TcpConnection();

private:
    EventLoop* loop_;
    string name_;
    bool reading_;
    Socket tcpConnectionSocket_;
    Channel tcpConnectionChannel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;

    ConnectionCallback connectionCallback_;

};