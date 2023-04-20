
#pragma once

#include "base/copyable.h"
#include "base/Types.h"

#include <boost/operators.hpp>

class Timestamp : public copyable,
                  public boost::equality_comparable<Timestamp>,
                  public boost::less_than_comparable<Timestamp>
{
public:
    Timestamp()
        :
        microSecondsSinceEpoch_(0)
    {}

    explicit Timestamp(int64_t microSecondsSinceEpochArg)
        :
        microSecondsSinceEpoch_(microSecondsSinceEpochArg)
    {}

    void swap(Timestamp& rhs)
    {
        std::swap(microSecondsSinceEpoch_, rhs.microSecondsSinceEpoch_);
    }

    string toString() const;
    string toFormattedString(bool showMicrosecond = true) const;

    bool isVaild() const
    {
        return microSecondsSinceEpoch_ > 0;
    }

    int64_t microSecondsSinceEpoch() const
    {
        return microSecondsSinceEpoch_;
    }

    time_t secondsSinceEpoch() const
    {
        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }

    static Timestamp now();
    static Timestamp setInvaild()
    {
        return Timestamp(); // == 0
    }

    static Timestamp fromUnixTime(time_t t, int microseconds)
    {
        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }

    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t, 0);
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(Timestamp high, Timestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp t, double s)
{
    int64_t add = static_cast<int64_t>(s * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(t.microSecondsSinceEpoch() + add);
}