#pragma once

#include <cstdint>

struct TimeSpan
{
    using StorageType = std::int64_t;

    TimeSpan() = default;
    explicit TimeSpan(StorageType milliseconds) : milliseconds(milliseconds) {}
    static TimeSpan from_seconds(double seconds) { return TimeSpan(static_cast<StorageType>(seconds * 1000.0)); }

    inline TimeSpan& operator+=(const TimeSpan& rhs)
    {
        milliseconds += rhs.milliseconds;
        return *this;
    }

    inline TimeSpan& operator-=(const TimeSpan& rhs)
    {
        milliseconds -= rhs.milliseconds;
        return *this;
    }

    inline TimeSpan& operator*=(const TimeSpan& rhs)
    {
        milliseconds *= rhs.milliseconds;
        return *this;
    }

    inline TimeSpan& operator/=(const TimeSpan& rhs)
    {
        milliseconds /= rhs.milliseconds;
        return *this;
    }

    StorageType milliseconds = 0;
};

inline bool operator==(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return lhs.milliseconds == rhs.milliseconds;
}

inline bool operator!=(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return lhs.milliseconds != rhs.milliseconds;
}

inline bool operator<=(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return lhs.milliseconds <= rhs.milliseconds;
}

inline bool operator<(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return lhs.milliseconds < rhs.milliseconds;
}

inline bool operator>=(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return lhs.milliseconds >= rhs.milliseconds;
}

inline bool operator>(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return lhs.milliseconds > rhs.milliseconds;
}

inline TimeSpan operator+(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return TimeSpan(lhs.milliseconds + rhs.milliseconds);
}

inline TimeSpan operator-(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return TimeSpan(lhs.milliseconds - rhs.milliseconds);
}

inline TimeSpan operator*(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return TimeSpan(lhs.milliseconds * rhs.milliseconds);
}

inline TimeSpan operator/(const TimeSpan& lhs, const TimeSpan& rhs)
{
    return TimeSpan(lhs.milliseconds / rhs.milliseconds);
}
