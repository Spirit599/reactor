

add_executable(testEventLoop testEventLoop.cc)
target_link_libraries(testEventLoop net)

add_executable(testPoller testPoller.cc)
target_link_libraries(testPoller net)

add_executable(testSocketOps testSocketOps.cc)
target_link_libraries(testSocketOps net)

add_executable(testAcceptor testAcceptor.cc)
target_link_libraries(testAcceptor net)

add_executable(testEventLoopThread testEventLoopThread.cc)
target_link_libraries(testEventLoopThread net)

add_executable(testEventLoopThreadPool testEventLoopThreadPool.cc)
target_link_libraries(testEventLoopThreadPool net)

add_executable(testTcpServer testTcpServer.cc)
target_link_libraries(testTcpServer net)

add_executable(testTimer testTimer.cc)
target_link_libraries(testTimer net)

add_executable(testTimerQueue testTimerQueue.cc)
target_link_libraries(testTimerQueue net)