#pragma once

#include "Vec2.h"

namespace math
{
    template<typename T>
    struct Mat22
    {
        T buffer[4];

        inline Mat22() {}
        inline Mat22(T val_0, T val_1, T val_2, T val_3)
        {
            buffer[0] = val_0;
            buffer[1] = val_1;
            buffer[2] = val_2;
            buffer[3] = val_3;
        }

        inline Mat22& operator+=(const Mat22& rhs)
        {
            buffer[0] += rhs.buffer[0];
            buffer[1] += rhs.buffer[1];
            buffer[2] += rhs.buffer[2];
            buffer[3] += rhs.buffer[3];
            return *this;
        }

        inline Mat22& operator-=(const Mat22& rhs)
        {
            buffer[0] -= rhs.buffer[0];
            buffer[1] -= rhs.buffer[1];
            buffer[2] -= rhs.buffer[2];
            buffer[3] -= rhs.buffer[3];
            return *this;
        }

        inline Mat22& operator*=(const Mat22& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        inline Mat22& operator/=(const Mat22& rhs)
        {
            *this = *this / rhs;
            return *this;
        }

        inline Mat22& operator*=(T factor)
        {
            buffer[0] *= factor;
            buffer[1] *= factor;
            buffer[2] *= factor;
            buffer[3] *= factor;
            return *this;
        }

        inline Mat22& operator/=(T factor)
        {
            buffer[0] /= factor;
            buffer[1] /= factor;
            buffer[2] /= factor;
            buffer[3] /= factor;
            return *this;
        }

        inline Mat22& operator%=(int factor)
        {
            buffer[0] %= factor;
            buffer[1] %= factor;
            buffer[2] %= factor;
            buffer[3] %= factor;
            return *this;
        }

        inline Mat22 operator+(const Mat22& rhs) const
        {
            Mat22 m = *this; m += rhs; return m;
        }

        inline Mat22 operator-(const Mat22& rhs) const
        {
            Mat22 m = *this; m -= rhs; return m;
        }

        inline Mat22 operator*(const Mat22& rhs) const
        {
            Mat22 m;
            m.buffer[0] = buffer[0] * rhs.buffer[0] + buffer[1] * rhs.buffer[2];
            m.buffer[1] = buffer[0] * rhs.buffer[1] + buffer[1] * rhs.buffer[3];
            m.buffer[2] = buffer[2] * rhs.buffer[0] + buffer[3] * rhs.buffer[2];
            m.buffer[3] = buffer[2] * rhs.buffer[1] + buffer[3] * rhs.buffer[3];
            return m;
        }

        inline Mat22 operator/(const Mat22& rhs) const
        {
            Mat22 m;
            m.buffer[0] = buffer[0] / rhs.buffer[0] + buffer[1] / rhs.buffer[2];
            m.buffer[1] = buffer[0] / rhs.buffer[1] + buffer[1] / rhs.buffer[3];
            m.buffer[2] = buffer[2] / rhs.buffer[0] + buffer[3] / rhs.buffer[2];
            m.buffer[3] = buffer[2] / rhs.buffer[1] + buffer[3] / rhs.buffer[3];
            return m;
        }

        inline Mat22 operator*(T factor) const
        {
            Mat22 m = *this; m *= factor; return m;
        }

        inline Mat22 operator/(T factor) const
        {
            Mat22 m = *this; m /= factor; return m;
        }

        inline Vec2<T> operator*(const Vec2<T>& rhs) const
        {
            Vec2<T> r;
            r.x = buffer[0] * rhs.x + buffer[1] * rhs.y;
            r.y = buffer[2] * rhs.x + buffer[3] * rhs.y;
            return r;
        }

        inline bool operator==(const Mat22& rhs) const
        {
            return buffer[0] == rhs.buffer[0]
                && buffer[1] == rhs.buffer[1]
                && buffer[2] == rhs.buffer[2]
                && buffer[3] == rhs.buffer[3];
        }

        inline bool operator!=(const Mat22& rhs) const
        {
            return buffer[0] != rhs.buffer[0]
                || buffer[1] != rhs.buffer[1]
                || buffer[2] != rhs.buffer[2]
                || buffer[3] != rhs.buffer[3];
        }
    };

    typedef Mat22<float> Mat22f;
    typedef Mat22<int> Mat22i;
}
