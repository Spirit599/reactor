#pragma once

#include "net/TcpConnection.h"


class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient : public noncopyable
{
public:
    TcpClient(EventLoop* loop, const InetAddress& serverAddr, const string& nameArg);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const
    {
        std::unique_lock<std::mutex> ulk(mutex_);
        return connection_;
    }

    EventLoop* getLoop() const
    { return loop_; }

    const string& name() const
    { return name_; }

    void setConnectionCallback(ConnectionCallback cb)
    { connectionCallback_ = std::move(cb); }

    void setMessageCallback(MessageCallback cb)
    { messageCallback_ = std::move(cb); }



private:

    void newConnection(int fd);

    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    ConnectorPtr connector_;
    const string name_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    bool connect_;
    int nextConnId_;
    mutable std::mutex mutex_;
    TcpConnectionPtr connection_;
};