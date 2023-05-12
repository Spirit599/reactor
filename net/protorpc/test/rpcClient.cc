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
        LOG_INFO("onConnection(const TcpConnectionPtr& conn)");
        if(conn->connected())
        {
            channel_->setConnection(conn);
            PrimeRequest req;
            int num = rand() % 64 + 2;
            req.set_num(num);
            PrimeResponse* resp = new PrimeResponse();

            stub_.Solve(NULL, &req, resp, NewCallback(this, &RpcClient::solved, resp, num));
        }
        else
        {
            loop_->quit();
        }
    }

    void solved(PrimeResponse* resp, int num)
    {
        LOG_INFO("solved\n%d %s", num, resp->DebugString().c_str());
        client_.disconnect();
    }

    EventLoop* loop_;
    TcpClient client_;
    RpcChannelPtr channel_;
    PrimeService::Stub stub_;
};

int main(int argc, char const *argv[])
{
    srand(time(0));
    EventLoop loop;
    InetAddress serverAddr("127.0.0.1", 10013);

    RpcClient rpcClient(&loop, serverAddr);
    rpcClient.connect();

    loop.loop();

    return 0;
}
