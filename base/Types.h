#pragma once


#include <string>
#include <functional>
#include <memory>

using std::string;

typedef std::function<void()> Functor;
typedef std::function<void()> EventCallback;


class InetAddress;
typedef std::function<void(int fd, const InetAddress&)> NewConntionCallback;

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;

using std::placeholders::_1;
using std::placeholders::_2;