add_custom_command(OUTPUT rpc.pb.cc rpc.pb.h
  COMMAND protoc
  ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/rpc.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS rpc.proto
  VERBATIM )

include_directories(${PROJECT_BINARY_DIR})


add_library(protorpc rpc.pb.cc RpcCodec.cc RpcChannel.cc RpcServer.cc)
target_link_libraries(protorpc protobufCodec)

add_subdirectory(test)
