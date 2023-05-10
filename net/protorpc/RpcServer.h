#pragma once

#include "net/TcpServer.h"

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
    { server_.start(); }

private:

    void onConnection(const TcpConnectionPtr& conn);

    TcpServer server_;
    std::map<std::string, google::protobuf::Service*> services_;
};