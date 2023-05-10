#pragma once

#include "base/Timestamp.h"
#include "net/protobuf/ProtobufCodec.h"

class RpcMessage;
typedef std::shared_ptr<RpcMessage> RpcMessagePtr;

extern const char rpctag[]; //"RPC0"


typedef ProtobufCodecT<RpcMessage, rpctag> RpcCodec;

