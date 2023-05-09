#pragma once

#include "base/noncopyable.h"
#include "base/Timestamp.h"
#include "base/Types.h"

#include <memory>

namespace google
{
    namespace protobuf
    {
        class Message;
    }
}

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;


class ProtobufCodec : public noncopyable
{
public:
    const static int kHeaderLen = sizeof(int32_t);
    const static int kChecksumLen = sizeof(int32_t);
    const static int kMaxMessageLen = 64 * 1024 * 1024;

    enum ErrorCode
    {
        kNoError = 0,
        kInvalidLength,
        kCheckSumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError,
    };

    typedef std::function<bool(const TcpConnectionPtr&, const char* , int len, Timestamp)> RawMessageCallback;
    typedef std::function<void(const TcpConnectionPtr&, const MessagePtr&, Timestamp)> ProtobufMessageCallback;
    typedef std::function<void(const TcpConnectionPtr&, Buffer*, Timestamp, ErrorCode)> ErrorCallback;

    ProtobufCodec(const google::protobuf::Message* protoType,
                    string tagArg,
                    const ProtobufMessageCallback& messageCb,
                    const RawMessageCallback& rawCb = RawMessageCallback(),
                    const ErrorCallback& errorCb = defaultErrorCallback)
        :
        protoType_(protoType),
        tag_(tagArg),
        MessageCallback_(messageCb),
        rawCallback_(rawCb),
        errorCallback_(errorCb),
        kMinMessageLen(tagArg.size() + kChecksumLen)
    {}

    virtual ~ProtobufCodec() = default;

    const string& tag() const { return tag_; }

    void send(const TcpConnectionPtr& conn, const google::protobuf::Message& message);

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);

    virtual bool parseFromBuffer(const char* data, int len, google::protobuf::Message* message);

    virtual int serializeToBuffer(const google::protobuf::Message& message, Buffer* buf);


    static const string& errorCodeToString(ErrorCode errorCode);
    static void defaultErrorCallback(const TcpConnectionPtr&, Buffer*, Timestamp, ErrorCode);
    static int32_t checksum(const void* buf, int len);
    static bool validateCheckSum(const char* buf, int len);
    static int32_t asInt32(const char* buf);

    ErrorCode parse(const char* buf, int len, google::protobuf::Message* message);
    void fillEmptyBuffer(Buffer* buf, const google::protobuf::Message& message);


private:
    const google::protobuf::Message* protoType_;
    const string tag_;
    ProtobufMessageCallback MessageCallback_;
    RawMessageCallback rawCallback_;
    ErrorCallback errorCallback_;
    const int kMinMessageLen;
};