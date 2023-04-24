#pragma once

#include "base/noncpoyable.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
#include "net/Channel.h"

class TcpConnection : public noncopyable,
                      public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop, const string& name, int fd,
                    const InetAddress& localAddr, const InetAddress& peerAddr);
    
    ~TcpConnection();

    const string& name() const
    { return name_; }

    bool connected() const
    { return state_ == KConnected; }

    bool disconnected() const
    { return state_ == KDisconnected; }

    const InetAddress& localAddr() const
    { return localAddr_; }

    const InetAddress& peerAddr() const
    { return peerAddr_; }

    void connectionEstablished();

    void connectionDestroyed();

private:

    enum StateE {
        KConnecting,
        KConnected,
        KDisconnecting,
        KDisconnected
    };

    void setState(StateE s)
    { state_ = s; }

    

    EventLoop* loop_;
    string name_;
    bool reading_;
    Socket tcpConnectionSocket_;
    Channel tcpConnectionChannel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    StateE state_;

    ConnectionCallback connectionCallback_;

};