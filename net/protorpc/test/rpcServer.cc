#include "base/Log.h"
#include "net/protorpc/test/prime.pb.h"
#include "net/EventLoop.h"
#include "net/protorpc/RpcServer.h"


class PrimeServiceImpl : public PrimeService
{
public:

    PrimeServiceImpl(RpcServer* rpcServer)
        :
        rpcServer_(rpcServer)
    {}

    virtual void Solve(google::protobuf::RpcController* controller,
                        const PrimeRequest* req,
                        PrimeResponse* resp,
                        google::protobuf::Closure* done)
    {
        LOG_INFO("PrimeServiceImpl::Solve");
        // resp->set_isprime(checkPrime(req->num()));
        // done->Run();
        rpcServer_->threadPool().put(std::bind(&PrimeServiceImpl::doTask, this, req->num(), resp, done));
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

    void doTask(int num, PrimeResponse* resp, google::protobuf::Closure* done)
    {
        LOG_INFO("PrimeServiceImpl::doTask");
        resp->set_isprime(checkPrime(num));
        done->Run();
    }

private:
    RpcServer* rpcServer_;
};

int main(int argc, char const *argv[])
{
    
    EventLoop loop;
    InetAddress listenAddr(10013);
    
    RpcServer rpcServer(&loop, listenAddr);

    PrimeServiceImpl impl(&rpcServer);

    rpcServer.registerService(&impl);

    rpcServer.start();

    loop.loop();

    return 0;
}
