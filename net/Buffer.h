#pragma once

#include "base/copyable.h"
#include "base/Types.h"

#include <vector>
#include <algorithm>
#include <string.h>
#include <assert.h>

class Buffer : public copyable
{
public:
    static const size_t kPrepend = 8;
    static const size_t kInitSize = 1024;

    Buffer(size_t initSize = kInitSize)
        :
        buffer_(kPrepend + initSize),
        readIdx_(kPrepend),
        writeIdx_(kPrepend)
    {}

    void swap(Buffer& rhs)
    {
        buffer_.swap(rhs.buffer_);
        std::swap(readIdx_, rhs.readIdx_);
        std::swap(writeIdx_, rhs.writeIdx_);
    }

    size_t readableBytes() const
    { return writeIdx_ - readIdx_; }

    size_t writeableBytes() const
    { return buffer_.size() - writeIdx_; }

    size_t prepengableBytes() const
    { return readIdx_; }

    const char* peek() const
    { return begin() + readIdx_; }

    const char* last() const
    { return begin() + writeIdx_; }

    char* last() 
    { return begin() + writeIdx_; }

    const char* findCRLF() const
    {
        const char* crlf = std::search(peek(), last(), kCRLF, kCRLF + 2);
        return crlf != last() ? crlf : nullptr;
    }

    const char* findEOL() const
    {
        const void* eol = memchr(peek(), '\n', readableBytes());
        return static_cast<const char*>(eol);
    }

    void retrieveAll()
    {
        readIdx_ = kPrepend;
        writeIdx_ = kPrepend;
    }

    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if(len < readableBytes())
        {
            readIdx_ += len;
        }
        else
        {
            retrieveAll();
        }
    }

    string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        string ret(peek(), len);
        retrieve(len);
        return ret;
    }

    string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    void makeSpace(size_t len)
    {
        buffer_.resize(writeIdx_ + len); // todo improve
    }

    void checkSpace(size_t len)
    {
        if(writeableBytes() < len)
        {
            makeSpace(len);
        }
    }

    void hasWritten(size_t len)
    {
        writeIdx_ += len;
    }

    void append(const char* data, size_t len)
    {
        checkSpace(len);
        std::copy(data, data + len, last());
        hasWritten(len);
    }

    void append(const void* data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void appendInt32(int32_t x)
    {
        int32_t be32 = htobe32(x);
        append(&be32, sizeof(be32));
    }

    void fillPrepend(const void* data, size_t len)
    {
        assert(len <= prepengableBytes());
        readIdx_ -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d + len, begin() + readIdx_);
    }

    int32_t peekInt32() const
    {
        assert(readableBytes() >= sizeof(int32_t));
        int32_t be32 = 0;
        memcpy(&be32, peek(), sizeof(be32));
        return be32toh(be32);
    }


    ssize_t readFd(int fd, int* savedErrno);

private:

    const char* begin() const
    { return &*buffer_.begin(); }

    char* begin() 
    { return &*buffer_.begin(); }

    std::vector<char> buffer_;
    size_t readIdx_;
    size_t writeIdx_;

    static const char kCRLF[];
};