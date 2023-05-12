#pragma once

#include "base/Timestamp.h"
#include "net/protobuf/ProtobufCodec.h"

// class Buffer;
// class TcpConnection;
// typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

class RpcMessage;
typedef std::shared_ptr<RpcMessage> RpcMessagePtr;

extern const char rpctag[]; //"RPC0"


typedef ProtobufCodecT<RpcMessage, rpctag> RpcCodec;

