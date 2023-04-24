#include "net/Buffer.h"

#include <sys/uio.h>

const char Buffer::kCRLF[] = "\r\n";
const size_t Buffer::kPrepend;
const size_t Buffer::kInitSize;

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    char extraBuf[65536];
    int iovcnt = 2;
    struct iovec bufs[iovcnt];

    const size_t writeAble = writeableBytes();
    bufs[0].iov_base = last();
    bufs[0].iov_len = writeAble;
    bufs[1].iov_base = extraBuf;
    bufs[1].iov_len = sizeof(extraBuf);

    
    const ssize_t n = ::readv(fd, bufs, iovcnt);
    if(n < 0)
    {
        *savedErrno = errno;
    }
    else if(static_cast<size_t>(n) <= writeAble)
    {
        hasWritten(n);
    }
    else
    {
        hasWritten(writeAble);
        append(extraBuf, n - writeAble);
    }
    return n;
}
