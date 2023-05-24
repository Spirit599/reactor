#pragma once

#include "net/TcpServer.h"
#include "base/ThreadPool.h"

namespace google
{
    namespace protobuf
    {
        class Service;
    }
}

class RpcServer
{
public:
    RpcServer(EventLoop* loop, const InetAddress& listenAddr);

    void setThreadNum(int num)
    { server_.setThreadNum(num); }

    void registerService(google::protobuf::Service*);

    void start()
    { server_.start(); threadPool_.start(4); }

    ThreadPool& threadPool()
    { return threadPool_; }

private:

    void onConnection(const TcpConnectionPtr& conn);

    TcpServer server_;
    ThreadPool threadPool_;

    std::map<std::string, google::protobuf::Service*> services_;
};