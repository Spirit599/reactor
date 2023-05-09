#pragma once

#include "base/noncopyable.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
#include "net/Channel.h"
#include "net/Buffer.h"

class TcpConnection : public noncopyable,
                      public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop, const string& name, int fd,
                    const InetAddress& localAddr, const InetAddress& peerAddr);
    
    ~TcpConnection();

    const string& name() const
    { return name_; }

    Buffer& inputBuffer()
    { return inputBuffer_; }

    Buffer& outputBuffer()
    { return outputBuffer_; }

    bool connected() const
    { return state_ == KConnected; }

    bool disconnected() const
    { return state_ == KDisconnected; }

    const InetAddress& localAddr() const
    { return localAddr_; }

    const InetAddress& peerAddr() const
    { return peerAddr_; }

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }

    void connectionEstablished();

    void connectionDestroyed();

    void handleRead();
    void handleWrite();
    void handleClose();

    void sendMessage(const char* data, size_t len);
    void sendMessage(Buffer* buf);

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

    Buffer inputBuffer_;
    Buffer outputBuffer_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};