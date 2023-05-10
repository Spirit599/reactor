#pragma once


#include <string>
#include <functional>
#include <memory>

using std::string;
class Timestamp;

typedef std::function<void()> Functor;
typedef std::function<void(Timestamp)> ReadEventCallback;
typedef std::function<void()> EventCallback;


class InetAddress;
typedef std::function<void(int fd, const InetAddress&)> NewConnectionCallback;

class Buffer;
class TcpConnection;


typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;


typedef std::function<void()> TimerCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, Timestamp);