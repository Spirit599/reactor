#pragma once

#include "base/noncopyable.h"
#include "net/InetAddress.h"

#include <functional>
#include <memory>

class Channel;
class EventLoop;


class Connector : public noncopyable, public std::enable_shared_from_this<Connector>
{
public:
    typedef std::function<void(int fd)> NewConnectionCallback;

    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    void start();
    // void restart
    void stop();

private:

    enum States {
        KConnecting,
        KConnected,
        KDisconnected
    };

    void setState(States s)
    { state_ = s; }

    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int fd);
    void handleWrite();
    // void handleError();

    int removeAndResetChannel();
    void resetChannel();


    EventLoop* loop_;
    InetAddress serverAddr_;
    bool connect_;
    States state_;
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    
};