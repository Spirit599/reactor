


set(net_SRCS
  Poller.cc
  EventLoop.cc
  Channel.cc
  poller/EpollPoller.cc
  InetAddress.cc
  SocketOps.cc
  Socket.cc
  Acceptor.cc
  EventLoopThread.cc
  EventLoopThreadPool.cc
  TcpConnection.cc
  TcpServer.cc
)

add_library(net ${net_SRCS})
target_link_libraries(net base)

add_subdirectory(test)