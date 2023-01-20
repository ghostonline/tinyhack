#pragma once

#include <cmath>

namespace math
{
    template<typename T>
    struct Vec4
    {
        T x, y, z, w;

        Vec4() : x(0), y(0), z(0), w(1) {}
        Vec4(T x, T y, T z) : x(x), y(y), z(z), w(1) {}
        Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

        inline Vec4 operator-() const
        {
            return{
                -x,
                -y,
                -z,
                -w,
            };
        }

        inline Vec4& operator+=(const Vec4& rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            this->w += rhs.w;
            return *this;
        }

        inline Vec4& operator-=(const Vec4& rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            this->z -= rhs.z;
            this->w -= rhs.w;
            return *this;
        }

        inline Vec4& operator*=(T factor)
        {
            this->x *= factor;
            this->y *= factor;
            this->z *= factor;
            this->w *= factor;
            return *this;
        }

        inline Vec4& operator/=(T factor)
        {
            this->x /= factor;
            this->y /= factor;
            this->z /= factor;
            this->w /= factor;
            return *this;
        }

        inline Vec4 operator+(const Vec4& rhs) const
        {
            return
            { this->x + rhs.x
            , this->y + rhs.y
            , this->z + rhs.z
            , this->w + rhs.w
            };
        }

        inline Vec4 operator-(const Vec4& rhs) const
        {
            return
            { this->x - rhs.x
            , this->y - rhs.y
            , this->z - rhs.z
            , this->w - rhs.w
            };
        }

        inline Vec4 operator*(T factor) const
        {
            return
            { this->x * factor
            , this->y * factor
            , this->z * factor
            , this->w * factor
            };
        }

        inline Vec4 operator/(T factor) const
        {
            return
            { this->x / factor
            , this->y / factor
            , this->z / factor
            , this->w / factor
            };
        }

        inline bool operator==(const Vec4& rhs) const
        {
            return this->x == rhs.x
                && this->y == rhs.y
                && this->z == rhs.z
                && this->w == rhs.w
                ;
        }

        inline bool operator!=(const Vec4& rhs) const
        {
            return this->x != rhs.x
                || this->y != rhs.y
                || this->z != rhs.z
                || this->w != rhs.w
                ;
        }

        inline Vec4& set(T new_x, T new_y, T new_z, T new_w = 1)
        {
            x = new_x;
            y = new_y;
            z = new_z;
            w = new_w;
            return *this;
        }

    };

    template<typename T>
    inline static float length2(const Vec4<T>& p)
    {
        return p.x * p.x + p.y * p.y + p.z * p.z + p.w * p.w;
    }

    template<typename T>
    inline static float length(const Vec4<T>& p)
    {
        return std::sqrt(length2(p));
    }

    template<typename T>
    inline static float distance2(const Vec4<T>& a, const Vec4<T>& b)
    {
        return length2(b - a);
    }

    template<typename T>
    inline static float distance(const Vec4<T>& a, const Vec4<T>& b)
    {
        return length(b - a);
    }

    template<typename T>
    inline static void normalize(Vec4<T>* v)
    {
        *v /= length(*v);
    }

    template<typename T>
    inline static Vec4<T> normalized(const Vec4<T>& v)
    {
        Vec4<T> vec = v;
        normalize(&vec);
        return vec;
    }

    template<typename T>
    inline static float dot(const Vec4<T>& a, const Vec4<T>& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    typedef Vec4<float> Vec4f;
    typedef Vec4<int> Vec4i;
}
