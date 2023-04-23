

add_executable(testEventLoop testEventLoop.cc)
target_link_libraries(testEventLoop net)

add_executable(testPoller testPoller.cc)
target_link_libraries(testPoller net)

add_executable(testSocketOps testSocketOps.cc)
target_link_libraries(testSocketOps net)

add_executable(testAcceptor testAcceptor.cc)
target_link_libraries(testAcceptor net)