#include "net/protorpc/RpcServer.h"

#include "base/Log.h"
#include "net/protorpc/RpcChannel.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/service.h>

RpcServer::RpcServer(EventLoop* loop, const InetAddress& listenAddr)
    :
    server_(loop, listenAddr, "RpcServer"),
    threadPool_(128, "RpcServerThreadPool")
{
    server_.setConnectionCallback(std::bind(&RpcServer::onConnection, this, _1));
}

void RpcServer::registerService(google::protobuf::Service* service)
{
    const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
    services_[desc->full_name()] = service;
}

void RpcServer::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        LOG_TRACE("rpc %s->%s is UP", conn->localAddr().addrToIpAddr().c_str(), conn->peerAddr().addrToIpAddr().c_str());
        RpcChannelPtr channel(new RpcChannel(conn));
        channel->setServices(&services_);
        conn->setMessageCallback(std::bind(&RpcChannel::onMessage, channel.get(), _1, _2, _3));
        conn->setContext(channel);

    }
    else
    {
        conn->setContext(RpcChannelPtr());
        LOG_TRACE("rpc %s->%s is DOWN", conn->localAddr().addrToIpAddr().c_str(), conn->peerAddr().addrToIpAddr().c_str());
    }
}