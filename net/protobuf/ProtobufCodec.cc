#include "net/protobuf/ProtobufCodec.h"

#include "base/Log.h"
#include "net/TcpConnection.h"

#include <google/protobuf/message.h>
#include <zlib.h>



// int ProtobufVersionCheck()
// {
//     GOOGLE_PROTOBUF_VERIFY_VERSION;
//     return 0;
// }

void ProtobufCodec::send(const TcpConnectionPtr& conn, const google::protobuf::Message& message)
{
    Buffer buf;
    fillEmptyBuffer(&buf, message);
    conn->sendMessage(&buf);
}

void ProtobufCodec::fillEmptyBuffer(Buffer* buf, const google::protobuf::Message& message)
{
    assert(buf->readableBytes() == 0);

    buf->append(tag_.data(), tag_.size());
    int byteSize = serializeToBuffer(message, buf);

    int32_t checkSum = checksum(buf->peek(), static_cast<int>(buf->readableBytes()));
    buf->appendInt32(checkSum);
    assert(buf->readableBytes() == tag_.size() + byteSize + kChecksumLen);
    int32_t len = htobe32(static_cast<int32_t>(buf->readableBytes()));
    buf->fillPrepend(&len, sizeof(len));
}

void ProtobufCodec::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime)
{
    LOG_INFO("ProtobufCodec::onMessage");
    while(buf->readableBytes() >= static_cast<uint32_t>(kMinMessageLen + kHeaderLen))
    {
        const int32_t len = buf->peekInt32();
        if(len > kMaxMessageLen || len < kMinMessageLen)
        {
            errorCallback_(conn, buf, receiveTime, kInvalidLength);
            break;
        }
        else if(buf->readableBytes() >= static_cast<size_t>(kHeaderLen + len))
        {
            if(rawCallback_ && !rawCallback_(conn, buf->peek(), kHeaderLen + len, receiveTime))
            {
                buf->retrieve(kHeaderLen + len);
                continue;
            }

            MessagePtr messagePtr(protoType_->New());
            ErrorCode errorCode = parse(buf->peek() + kHeaderLen, len, messagePtr.get());
            if(errorCode == kNoError)
            {
                if(MessageCallback_)
                {
                    MessageCallback_(conn, messagePtr, receiveTime);
                    buf->retrieve(kHeaderLen + len);
                }
            }
            else
            {
                errorCallback_(conn, buf, receiveTime, errorCode);
                break;
            }
        }
        else
        {
            break;
        }
    }
}

bool ProtobufCodec::parseFromBuffer(const char* data, int len, google::protobuf::Message* message)
{
    return message->ParseFromArray(data, len);
}

int ProtobufCodec::serializeToBuffer(const google::protobuf::Message& message, Buffer* buf)
{
    int byteSize = message.ByteSize();
    buf->checkSpace(byteSize + kChecksumLen);

    uint8_t* start = reinterpret_cast<uint8_t*>(buf->last());
    uint8_t* end = message.SerializeWithCachedSizesToArray(start);

    buf->hasWritten(byteSize);
    return byteSize;
}

namespace
{
    const string kNoErrorStr = "NoError";
    const string kInvalidLengthStr = "InvalidLength";
    const string kCheckSumErrorStr = "CheckSumError";
    const string kInvalidNameLenStr = "InvalidNameLen";
    const string kUnknownMessageTypeStr = "UnknownMessageType";
    const string kParseErrorStr = "ParseError";
    const string kUnknownErrorStr = "UnknownError";
}

const string& ProtobufCodec::errorCodeToString(ErrorCode errorCode)
{
    switch (errorCode)
    {
        case kNoError:
             return kNoErrorStr;
        case kInvalidLength:
            return kInvalidLengthStr;
        case kCheckSumError:
            return kCheckSumErrorStr;
        case kInvalidNameLen:
            return kInvalidNameLenStr;
        case kUnknownMessageType:
            return kUnknownMessageTypeStr;
        case kParseError:
            return kParseErrorStr;
        default:
            return kUnknownErrorStr;
    }
}

void ProtobufCodec::defaultErrorCallback(const TcpConnectionPtr& conn, Buffer* buf, Timestamp t, ErrorCode errorCode)
{
    LOG_ERROR("ProtobufCodec::defaultErrorCallback %s", errorCodeToString(errorCode).c_str());
    if(conn && conn->connected())
    {
        //??? todo fixme
        conn->handleClose();
    }
}

int32_t ProtobufCodec::asInt32(const char* buf)
{
    int32_t be32 = 0;
    memcpy(&be32, buf, sizeof(be32));
    return be32toh(be32);
}

int32_t ProtobufCodec::checksum(const void* buf, int len)
{
    return static_cast<int32_t>(::adler32(1, static_cast<const Bytef*>(buf), len));
}

bool ProtobufCodec::validateCheckSum(const char* buf, int len)
{
    int32_t expectedCheckSum = asInt32(buf + len - kChecksumLen);
    int32_t checkSum = checksum(buf, len - kChecksumLen);
    return checkSum == expectedCheckSum;
}

ProtobufCodec::ErrorCode ProtobufCodec::parse(const char* buf, int len, google::protobuf::Message* message)
{
    ErrorCode errorCode = kNoError;

    if(validateCheckSum(buf, len))
    {
        if(::memcmp(buf, tag_.data(), tag_.size()) == 0)
        {
            const char* data = buf + tag_.size();
            int32_t dataLen = len - kChecksumLen - static_cast<int>(tag_.size());
            if(parseFromBuffer(data, dataLen, message))
            {
                errorCode = kNoError;
            }
            else
            {
                errorCode = kParseError;
            }
        }
        else
        {
            errorCode = kUnknownMessageType;
        }
    }
    else
    {
        errorCode = kCheckSumError;
    }
    return errorCode;
}