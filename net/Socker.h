#pragma once

#include "base/noncpoyable.h"

class Socket : public noncopyable
{
public:
    explicit Socket(int fd)
        :
        fd_(fd)
    {}

    ~Socket();

    int fd() const
    { return fd_; }

    

private:
    int fd_;
};