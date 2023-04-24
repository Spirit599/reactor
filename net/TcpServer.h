#pragma once

#include "base/Types.h"
#include "net/TcpConnection.h"
#include "net/EventLoopThreadPool.h"
#include "net/Acceptor.h"

#include <unordered_map>

class TcpServer : public noncopyable
{
public:
    TcpServer(EventLoop* loop, const InetAddress& localAddr, const string& name);
    ~TcpServer();

    void newConntion(int fd, const InetAddress& addr);

    void removeConntion(const TcpConnectionPtr& conn);

    void start();

    void setThreadNum(int numThread);
    

private:
    EventLoop* loop_;
    string name_;
    string ipPort_;

    Acceptor acceptor_;
    EventLoopThreadPool threadsPool_;

    bool started_;

    int nextConnId_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;

    std::unordered_map<string, TcpConnectionPtr> connections_;
};
