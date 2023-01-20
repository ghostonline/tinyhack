#pragma once

#include "Vec3.h"

namespace math
{
    template<typename T>
    struct Mat33
    {
        T buffer[9];

        inline Mat33() {}
        inline Mat33(T val_0, T val_1, T val_2, T val_3, T val_4, T val_5, T val_6, T val_7, T val_8)
        {
            buffer[0] = val_0;
            buffer[1] = val_1;
            buffer[2] = val_2;
            buffer[3] = val_3;
            buffer[4] = val_4;
            buffer[5] = val_5;
            buffer[6] = val_6;
            buffer[7] = val_7;
            buffer[8] = val_8;
        }

        inline Mat33& operator+=(const Mat33& rhs)
        {
            buffer[0] += rhs.buffer[0];
            buffer[1] += rhs.buffer[1];
            buffer[2] += rhs.buffer[2];
            buffer[3] += rhs.buffer[3];
            buffer[4] += rhs.buffer[4];
            buffer[5] += rhs.buffer[5];
            buffer[6] += rhs.buffer[6];
            buffer[7] += rhs.buffer[7];
            buffer[8] += rhs.buffer[8];
            return *this;
        }

        inline Mat33& operator-=(const Mat33& rhs)
        {
            buffer[0] -= rhs.buffer[0];
            buffer[1] -= rhs.buffer[1];
            buffer[2] -= rhs.buffer[2];
            buffer[3] -= rhs.buffer[3];
            buffer[4] -= rhs.buffer[4];
            buffer[5] -= rhs.buffer[5];
            buffer[6] -= rhs.buffer[6];
            buffer[7] -= rhs.buffer[7];
            buffer[8] -= rhs.buffer[8];
            return *this;
        }

        inline Mat33& operator*=(const Mat33& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        inline Mat33& operator/=(const Mat33& rhs)
        {
            *this = *this / rhs;
            return *this;
        }

        inline Mat33& operator*=(T factor)
        {
            buffer[0] *= factor;
            buffer[1] *= factor;
            buffer[2] *= factor;
            buffer[3] *= factor;
            buffer[4] *= factor;
            buffer[5] *= factor;
            buffer[6] *= factor;
            buffer[7] *= factor;
            buffer[8] *= factor;
            return *this;
        }

        inline Mat33& operator/=(T factor)
        {
            buffer[0] /= factor;
            buffer[1] /= factor;
            buffer[2] /= factor;
            buffer[3] /= factor;
            buffer[4] /= factor;
            buffer[5] /= factor;
            buffer[6] /= factor;
            buffer[7] /= factor;
            buffer[8] /= factor;
            return *this;
        }

        inline Mat33& operator%=(int factor)
        {
            buffer[0] %= factor;
            buffer[1] %= factor;
            buffer[2] %= factor;
            buffer[3] %= factor;
            buffer[4] %= factor;
            buffer[5] %= factor;
            buffer[6] %= factor;
            buffer[7] %= factor;
            buffer[8] %= factor;
            return *this;
        }

        inline Mat33 operator+(const Mat33& rhs) const
        {
            Mat33 m = *this; m += rhs; return m;
        }

        inline Mat33 operator-(const Mat33& rhs) const
        {
            Mat33 m = *this; m -= rhs; return m;
        }

        inline Mat33 operator*(const Mat33& rhs) const
        {
            Mat33 m;
            m.buffer[0] = buffer[0] * rhs.buffer[0] + buffer[1] * rhs.buffer[3] + buffer[2] * rhs.buffer[6];
            m.buffer[1] = buffer[0] * rhs.buffer[1] + buffer[1] * rhs.buffer[4] + buffer[2] * rhs.buffer[7];
            m.buffer[2] = buffer[0] * rhs.buffer[2] + buffer[1] * rhs.buffer[5] + buffer[2] * rhs.buffer[8];
            m.buffer[3] = buffer[3] * rhs.buffer[0] + buffer[4] * rhs.buffer[3] + buffer[5] * rhs.buffer[6];
            m.buffer[4] = buffer[3] * rhs.buffer[1] + buffer[4] * rhs.buffer[4] + buffer[5] * rhs.buffer[7];
            m.buffer[5] = buffer[3] * rhs.buffer[2] + buffer[4] * rhs.buffer[5] + buffer[5] * rhs.buffer[8];
            m.buffer[6] = buffer[6] * rhs.buffer[0] + buffer[7] * rhs.buffer[3] + buffer[8] * rhs.buffer[6];
            m.buffer[7] = buffer[6] * rhs.buffer[1] + buffer[7] * rhs.buffer[4] + buffer[8] * rhs.buffer[7];
            m.buffer[8] = buffer[6] * rhs.buffer[2] + buffer[7] * rhs.buffer[5] + buffer[8] * rhs.buffer[8];
            return m;
        }

        inline Mat33 operator/(const Mat33& rhs) const
        {
            Mat33 m;
            m.buffer[0] = buffer[0] / rhs.buffer[0] + buffer[1] / rhs.buffer[3] + buffer[2] / rhs.buffer[6];
            m.buffer[1] = buffer[0] / rhs.buffer[1] + buffer[1] / rhs.buffer[4] + buffer[2] / rhs.buffer[7];
            m.buffer[2] = buffer[0] / rhs.buffer[2] + buffer[1] / rhs.buffer[5] + buffer[2] / rhs.buffer[8];
            m.buffer[3] = buffer[3] / rhs.buffer[0] + buffer[4] / rhs.buffer[3] + buffer[5] / rhs.buffer[6];
            m.buffer[4] = buffer[3] / rhs.buffer[1] + buffer[4] / rhs.buffer[4] + buffer[5] / rhs.buffer[7];
            m.buffer[5] = buffer[3] / rhs.buffer[2] + buffer[4] / rhs.buffer[5] + buffer[5] / rhs.buffer[8];
            m.buffer[6] = buffer[6] / rhs.buffer[0] + buffer[7] / rhs.buffer[3] + buffer[8] / rhs.buffer[6];
            m.buffer[7] = buffer[6] / rhs.buffer[1] + buffer[7] / rhs.buffer[4] + buffer[8] / rhs.buffer[7];
            m.buffer[8] = buffer[6] / rhs.buffer[2] + buffer[7] / rhs.buffer[5] + buffer[8] / rhs.buffer[8];
            return m;
        }

        inline Mat33 operator*(T factor) const
        {
            Mat33 m = *this; m *= factor; return m;
        }

        inline Mat33 operator/(T factor) const
        {
            Mat33 m = *this; m /= factor; return m;
        }

        inline Vec3<T> operator*(const Vec3<T>& rhs) const
        {
            Vec3<T> r;
            r.x = buffer[0] * rhs.x + buffer[1] * rhs.y + buffer[2] * rhs.z;
            r.y = buffer[3] * rhs.x + buffer[4] * rhs.y + buffer[5] * rhs.z;
            r.z = buffer[6] * rhs.x + buffer[7] * rhs.y + buffer[8] * rhs.z;
            return r;
        }

        inline bool operator==(const Mat33& rhs) const
        {
            return buffer[0] == rhs.buffer[0]
                && buffer[1] == rhs.buffer[1]
                && buffer[2] == rhs.buffer[2]
                && buffer[3] == rhs.buffer[3]
                && buffer[4] == rhs.buffer[4]
                && buffer[5] == rhs.buffer[5]
                && buffer[6] == rhs.buffer[6]
                && buffer[7] == rhs.buffer[7]
                && buffer[8] == rhs.buffer[8];
        }

        inline bool operator!=(const Mat33& rhs) const
        {
            return buffer[0] != rhs.buffer[0]
                || buffer[1] != rhs.buffer[1]
                || buffer[2] != rhs.buffer[2]
                || buffer[3] != rhs.buffer[3]
                || buffer[4] != rhs.buffer[4]
                || buffer[5] != rhs.buffer[5]
                || buffer[6] != rhs.buffer[6]
                || buffer[7] != rhs.buffer[7]
                || buffer[8] != rhs.buffer[8];
        }
    };

    typedef Mat33<float> Mat33f;
    typedef Mat33<int> Mat33i;
}
