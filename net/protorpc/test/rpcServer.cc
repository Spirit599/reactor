#include "base/Log.h"
#include "net/protorpc/test/prime.pb.h"
#include "net/EventLoop.h"
#include "net/protorpc/RpcServer.h"


class PrimeServiceImpl : public PrimeService
{
public:
    virtual void Solve(google::protobuf::RpcController* controller,
                        const PrimeRequest* req,
                        PrimeResponse* resp,
                        google::protobuf::Closure* done)
    {
        LOG_INFO("PrimeServiceImpl::Solve");
        resp->set_isprime(checkPrime(req->num()));
        done->Run();
    }

    bool checkPrime(int num)
    {
        for(int i = 2; i * i <= num; ++i)
        {
            if(num % i == 0)
                return false;
        }
        return true;
    }

    
};

int main(int argc, char const *argv[])
{
    
    EventLoop loop;
    InetAddress listenAddr(10013);
    PrimeServiceImpl impl;

    RpcServer rpcServer(&loop, listenAddr);
    rpcServer.registerService(&impl);

    rpcServer.start();

    loop.loop();

    return 0;
}
