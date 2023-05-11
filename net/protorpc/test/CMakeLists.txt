


add_custom_command(OUTPUT prime.pb.cc prime.pb.h
  COMMAND protoc
  ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/prime.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS prime.proto
  VERBATIM )


add_library(primeProto prime.pb.cc)
target_link_libraries(primeProto protobuf pthread)


include_directories(${PROJECT_BINARY_DIR})

add_executable(rpcClient rpcClient.cc)
target_link_libraries(rpcClient protorpc primeProto)


add_executable(rpcServer rpcServer.cc)
target_link_libraries(rpcServer protorpc primeProto)

