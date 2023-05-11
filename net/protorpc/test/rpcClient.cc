#include "base/Log.h"
#include "net/protorpc/test/prime.pb.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/TcpClient.h"
#include "net/TcpConnection.h"
#include "net/protorpc/RpcChannel.h"


class RpcClient : public noncopyable
{
public:
    RpcClient(EventLoop* loop, const InetAddress& serverAddr)
        :
        loop_(loop),
        client_(loop, serverAddr, "rpcClient"),
        channel_(new RpcChannel()),
        stub_(channel_.get())
    {
        client_.setConnectionCallback(std::bind(&RpcClient::onConnection, this, _1));
        client_.setMessageCallback(std::bind(&RpcChannel::onMessage, channel_.get(), _1, _2, _3));
    }

    void connect()
    {
        client_.connect();
    }

private:

    void onConnection(const TcpConnectionPtr& conn)
    {
        if(conn->connected())
        {
            LOG_INFO("111111111111111111");
            channel_->setConnection(conn);
            PrimeRequest req;
            req.set_num(5);
            PrimeResponse* resp = new PrimeResponse();

            stub_.Solve(NULL, &req, resp, NewCallback(this, &RpcClient::solved, resp));
        }
        else
        {
            LOG_INFO("22222222222222222");
            loop_->quit();
        }
    }

    void solved(PrimeResponse* resp)
    {
        LOG_INFO("solved\n%s", resp->DebugString().c_str());
        client_.disconnect();
    }

    EventLoop* loop_;
    TcpClient client_;
    RpcChannelPtr channel_;
    PrimeService::Stub stub_;
};

int main(int argc, char const *argv[])
{
    EventLoop loop;
    InetAddress serverAddr("127.0.0.1", 8888);

    RpcClient rpcClient(&loop, serverAddr);
    rpcClient.connect();

    loop.loop();

    return 0;
}
