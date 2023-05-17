#include "net/protorpc/RpcChannel.h"

#include "net/protorpc/rpc.pb.h"
#include "base/Log.h"
// #include "net/Buffer.h"

#include <google/protobuf/descriptor.h>
// #include <functional>

RpcChannel::RpcChannel()
    :
    codec_(std::bind(&RpcChannel::onRpcMessage, this, _1, _2, _3)),
    services_(NULL)
{
    LOG_INFO("RpcChannel() %p", this);
}

RpcChannel::RpcChannel(const TcpConnectionPtr& conn)
    :
    codec_(std::bind(&RpcChannel::onRpcMessage, this, _1, _2, _3)),
    conn_(conn),
    services_(NULL)
{
    LOG_INFO("RpcChannel() %p", this);
}

RpcChannel::~RpcChannel()
{
    LOG_INFO("~RpcChannel() %p", this);
    for(const auto& outstanding : outstandings_)
    {
        OutstandingCall out = outstanding.second;
        delete out.response;
        delete out.done;
    }
}

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller,
                    const google::protobuf::Message* req,
                    google::protobuf::Message* response,
                    google::protobuf::Closure* done)
{
    RpcMessage message;
    message.set_type(REQUEST);
    int64_t id = id_.fetch_add(1);
    message.set_id(id);
    message.set_service(method->service()->full_name());
    message.set_method(method->name());
    message.set_request(req->SerializeAsString());

    LOG_INFO("CallMethod()");

    OutstandingCall out = { response, done };
    
    mutex_.lock();
    outstandings_[id] = out;
    mutex_.unlock();

    codec_.send(conn_, message);
}

void RpcChannel::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
    LOG_INFO("RpcChannel::onMessage");
    codec_.onMessage(conn, buf, receiveTime);
}

void RpcChannel::onRpcMessage(const TcpConnectionPtr& conn, const RpcMessagePtr& messagePtr, Timestamp receiveTime)
{

    LOG_INFO("RpcChannel::onRpcMessage");
    assert(conn == conn_);

    RpcMessage& message = *messagePtr;
    if(message.type() == RESPONSE)
    {
        int64_t id = message.id();
        assert(message.has_response() || message.has_error());

        OutstandingCall out = {NULL, NULL};

        {
            mutex_.lock();
            auto it = outstandings_.find(id);
            if(it != outstandings_.end())
            {
                out = it->second;
                outstandings_.erase(it);
            }
            mutex_.unlock();
        }

        if(out.response)
        {
            std::unique_ptr<google::protobuf::Message> dd(out.response);
            if(message.has_response())
            {
                out.response->ParseFromString(message.response());
            }
            if(out.done)
            {
                out.done->Run();
            }
        }
    }
    else if(message.type() == REQUEST)
    {
        ErrorCode error = WRONG_PROTO;
        if(services_)
        {
            auto it = services_->find(message.service());
            if(it != services_->end())
            {
                google::protobuf::Service* service = it->second;
                assert(service != NULL);
                
                const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
                const google::protobuf::MethodDescriptor* method = desc->FindMethodByName(message.method());

                if(method)
                {
                    std::unique_ptr<google::protobuf::Message> req(service->GetRequestPrototype(method).New());
                    if(req->ParseFromString(message.request()))
                    {
                        google::protobuf::Message* response = service->GetResponsePrototype(method).New();
                        int64_t id = message.id();
                        service->CallMethod(method, NULL, req.get(), response,
                                            NewCallback(this, &RpcChannel::doneCallback, response, id));
                        
                        error = NO_ERROR;
                    }
                    else
                    {
                        error = INVALID_REQUEST;
                    }
                }
                else
                {
                    error = NO_METHOD;
                }
            }
            else
            {
                error = NO_SERVICE;
            }
        }
        else
        {
            error = NO_SERVICE;
        }

        if(error != NO_ERROR)
        {
            RpcMessage response;
            response.set_type(RESPONSE);
            response.set_id(message.id());
            response.set_error(error);
            codec_.send(conn_, response);
        }
    }
}

void RpcChannel::doneCallback(google::protobuf::Message* response, int64_t id)
{
    std::unique_ptr<google::protobuf::Message> dd(response);
    RpcMessage message;
    message.set_type(RESPONSE);
    message.set_id(id);
    message.set_response(response->SerializeAsString());
    codec_.send(conn_, message);
}