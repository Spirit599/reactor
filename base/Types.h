#pragma once


#include <string>
#include <functional>

using std::string;

typedef std::function<void()> Functor;
typedef std::function<void()> EventCallback;


class InetAddress;
typedef std::function<void(int fd, const InetAddress&)> NewConntionCallback;

class TcpConnection;
typedef TcpConnection* TcpConnectionPtr;
typedef std::function<void(TcpConnectionPtr&)> ConnectionCallback;


using std::placeholders::_1;
using std::placeholders::_2;