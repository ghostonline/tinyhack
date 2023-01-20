#pragma once

#include <cmath>

namespace math
{
    template<typename T>
    struct Vec2
    {
        T x, y;

        Vec2() : x(0), y(0){}
        Vec2(T x, T y) : x(x), y(y) {}

        inline Vec2 operator-() const
        {
            return{
                -x,
                -y,
            };
        }

        inline Vec2& operator+=(const Vec2& rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }

        inline Vec2& operator-=(const Vec2& rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            return *this;
        }

        inline Vec2& operator*=(T factor)
        {
            this->x *= factor;
            this->y *= factor;
            return *this;
        }

        inline Vec2& operator/=(T factor)
        {
            this->x /= factor;
            this->y /= factor;
            return *this;
        }

        inline Vec2 operator+(const Vec2& rhs) const
        {
            return{ this->x + rhs.x, this->y + rhs.y };
        }

        inline Vec2 operator-(const Vec2& rhs) const
        {
            return{ this->x - rhs.x, this->y - rhs.y };
        }

        inline Vec2 operator*(T factor) const
        {
            return{ this->x * factor, this->y * factor };
        }

        inline Vec2 operator/(T factor) const
        {
            return{ this->x / factor, this->y / factor };
        }

        inline bool operator==(const Vec2& rhs) const
        {
            return this->x == rhs.x && this->y == rhs.y;
        }

        inline bool operator!=(const Vec2& rhs) const
        {
            return this->x != rhs.x || this->y != rhs.y;
        }

        inline Vec2& set(T new_x, T new_y)
        {
            x = new_x;
            y = new_y;
            return *this;
        }

        template<typename OtherT>
        inline static Vec2 cast(const Vec2<OtherT>& rhs)
        {
            return {
                static_cast<T>(rhs.x),
                static_cast<T>(rhs.y)
            };
        }

        static const Vec2<T> Zero;
        static const Vec2<T> One;
        static const Vec2<T> AxisX;
        static const Vec2<T> AxisY;
    };

    template<typename T>
    const Vec2<T> Vec2<T>::Zero = Vec2<T>(0, 0);

    template<typename T>
    const Vec2<T> Vec2<T>::One = Vec2<T>(1, 1);

    template<typename T>
    const Vec2<T> Vec2<T>::AxisX = Vec2<T>(1, 0);

    template<typename T>
    const Vec2<T> Vec2<T>::AxisY = Vec2<T>(0, 1);

    template<typename T>
    inline static T length2(const Vec2<T>& p)
    {
        return p.x * p.x + p.y * p.y;
    }

    template<typename T>
    inline static float length(const Vec2<T>& p)
    {
        return std::sqrt(static_cast<float>(length2(p)));
    }

    template<typename T>
    inline static T distance2(const Vec2<T>& a, const Vec2<T>& b)
    {
        return length2(b - a);
    }

    template<typename T>
    inline static float distance(const Vec2<T>& a, const Vec2<T>& b)
    {
        return length(b - a);
    }

    template<typename T>
    inline static void normalize(Vec2<T>* v)
    {
        *v /= length(*v);
    }

    template<typename T>
    inline static Vec2<T> normalized(const Vec2<T>& v)
    {
        Vec2<T> vec = v;
        normalize(&vec);
        return vec;
    }

    template<typename T>
    inline static float dot(const Vec2<T>& a, const Vec2<T>& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    template<typename T>
    inline static void rotate(Vec2<T>* v, float angle)
    {
        T x = std::cos(angle) * v->x - std::sin(angle) * v->y;
        T y = std::sin(angle) * v->x + std::cos(angle) * v->y;
        v->x = x;
        v->y = y;
    }

    template<typename T>
    inline static Vec2<T> project(const Vec2<T>& v, const Vec2<T>& axis)
    {
        // TODO: Test this thoroughly before use
        T dp = dot(v, axis);
        T l2 = length2(axis);
        return Vec2<T>(
            (dp / l2) * v.x,
            (dp / l2) * v.y
            );
    }

    template<typename T>
    inline static Vec2<T> perpendicular_ccw(const Vec2<T>& v)
    {
        return Vec2<T>(v.y, -v.x);
    }

    template<typename T>
    inline static Vec2<T> perpendicular_cw(const Vec2<T>& v)
    {
        return Vec2<T>(-v.y, v.x);
    }

    template<typename T>
    inline static double angle(const Vec2<T>& v)
    {
        return std::atan2(v.y, v.x);
    }

    inline static float angle(const Vec2<float>& v)
    {
        return std::atan2(v.y, v.x);
    }

    typedef Vec2<float> Vec2f;
    typedef Vec2<int> Vec2i;
}
