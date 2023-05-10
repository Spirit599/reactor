#pragma once

#include "net/protorpc/RpcCodec.h"

#include <google/protobuf/service.h>

#include <map>
#include <atomic>
#include <mutex>

namespace google
{
    namespace protobuf
    {
        class Message;
        class Descriptor;
        class MethodDescriptor;
        class ServiceDescriptor;

        class Closure;

        class RpcController;
        class Service;
    }
}


class RpcChannel : public google::protobuf::RpcChannel
{
public:
    RpcChannel();

    explicit RpcChannel(const TcpConnectionPtr& conn);

    ~RpcChannel() override;

    void setConnection(const TcpConnectionPtr& conn)
    { conn_ = conn; }

    void setServices(const std::map<std::string, google::protobuf::Service*>* services)
    { services_ = services; }

    void CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller,
                    const google::protobuf::Message* req,
                    google::protobuf::Message* response,
                    google::protobuf::Closure* done) override;
    
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);

private:

    void onRpcMessage(const TcpConnectionPtr& conn, const RpcMessagePtr& MessagePtr, Timestamp receiveTime);

    void doneCallback(google::protobuf::Message* response, int64_t id);

    RpcCodec codec_;
    TcpConnectionPtr conn_;
    std::atomic<int64_t> id_;
    std::mutex mutex_;

    struct OutstandingCall
    {
        google::protobuf::Message* response;
        google::protobuf::Closure* done;
    };

    std::map<int64_t, OutstandingCall> outstandings_;

    const std::map<std::string, google::protobuf::Service*>* services_;
};

typedef std::shared_ptr<RpcChannel> RpcChannelPtr;