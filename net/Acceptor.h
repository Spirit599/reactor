#pragma once

#include <functional>

#include "net/Channel.h"
#include "net/Socket.h"
#include "net/InetAddress.h"

class EventLoop;

class Acceptor : public noncopyable
{
public:
    Acceptor(EventLoop* loop, const InetAddress& addr);
    ~Acceptor();

    void startListen();
    
    bool isListening()
    { return listening_; }

private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    bool listening_;
    int idleFd_;
};