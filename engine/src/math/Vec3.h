#pragma once

#include <cmath>

namespace math
{
    template<typename T>
    struct Vec3
    {
        T x, y, z;

        Vec3() : x(0), y(0), z(0) {}
        Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

        inline Vec3 operator-() const
        {
            return{
                -x,
                -y,
                -z,
            };
        }

        inline Vec3& operator+=(const Vec3& rhs)
        {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            return *this;
        }

        inline Vec3& operator-=(const Vec3& rhs)
        {
            this->x -= rhs.x;
            this->y -= rhs.y;
            this->z -= rhs.z;
            return *this;
        }

        inline Vec3& operator*=(T factor)
        {
            this->x *= factor;
            this->y *= factor;
            this->z *= factor;
            return *this;
        }

        inline Vec3& operator/=(T factor)
        {
            this->x /= factor;
            this->y /= factor;
            this->z /= factor;
            return *this;
        }

        inline Vec3 operator+(const Vec3& rhs) const
        {
            return
            { this->x + rhs.x
            , this->y + rhs.y
            , this->z + rhs.z
            };
        }

        inline Vec3 operator-(const Vec3& rhs) const
        {
            return
            { this->x - rhs.x
            , this->y - rhs.y
            , this->z - rhs.z
            };
        }

        inline Vec3 operator*(T factor) const
        {
            return
            { this->x * factor
            , this->y * factor
            , this->z * factor
            };
        }

        inline Vec3 operator/(T factor) const
        {
            return
            { this->x / factor
            , this->y / factor
            , this->z / factor
            };
        }

        inline bool operator==(const Vec3& rhs) const
        {
            return this->x == rhs.x
                && this->y == rhs.y
                && this->z == rhs.z;
        }

        inline bool operator!=(const Vec3& rhs) const
        {
            return this->x != rhs.x
                || this->y != rhs.y
                || this->z != rhs.z;
        }

        inline Vec3& set(T new_x, T new_y, T new_z)
        {
            x = new_x;
            y = new_y;
            z = new_z;
            return *this;
        }

        static const Vec3<T> Zero;
        static const Vec3<T> One;
        static const Vec3<T> AxisX;
        static const Vec3<T> AxisY;
        static const Vec3<T> AxisZ;
    };

    template<typename T>
    const Vec3<T> Vec3<T>::Zero = Vec3<T>(0, 0, 0);

    template<typename T>
    const Vec3<T> Vec3<T>::One = Vec3<T>(1, 1, 1);

    template<typename T>
    const Vec3<T> Vec3<T>::AxisX = Vec3<T>(1, 0, 0);

    template<typename T>
    const Vec3<T> Vec3<T>::AxisY = Vec3<T>(0, 1, 0);

    template<typename T>
    const Vec3<T> Vec3<T>::AxisZ = Vec3<T>(0, 0, 1);

    template<typename T>
    inline static float length2(const Vec3<T>& p)
    {
        return p.x * p.x + p.y * p.y + p.z * p.z;
    }

    template<typename T>
    inline static float length(const Vec3<T>& p)
    {
        return std::sqrt(length2(p));
    }

    template<typename T>
    inline static float distance2(const Vec3<T>& a, const Vec3<T>& b)
    {
        return length2(b - a);
    }

    template<typename T>
    inline static float distance(const Vec3<T>& a, const Vec3<T>& b)
    {
        return length(b - a);
    }

    template<typename T>
    inline static void normalize(Vec3<T>* v)
    {
        *v /= length(*v);
    }

    template<typename T>
    inline static Vec3<T> normalized(const Vec3<T>& v)
    {
        Vec3<T> vec = v;
        normalize(&vec);
        return vec;
    }

    template<typename T>
    inline static float dot(const Vec3<T>& a, const Vec3<T>& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    template<typename T>
    inline static Vec3<T> cross(const Vec3<T>& a, const Vec3<T>& b)
    {
        return
        {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x,
        };
    }

    template<typename T>
    inline static void rotateAroundZ(Vec3<T>* v, float angle)
    {
        v->x = std::cos(angle) * v->x - std::sin(angle) * v->y;
        v->y = std::sin(angle) * v->x + std::cos(angle) * v->y;
    }

    template<typename T>
    inline static void rotateAroundY(Vec3<T>* v, float angle)
    {
        v->z = std::cos(angle) * v->z - std::sin(angle) * v->x;
        v->x = std::sin(angle) * v->z + std::cos(angle) * v->x;
    }

    template<typename T>
    inline static void rotateAroundX(Vec3<T>* v, float angle)
    {
        v->y = std::cos(angle) * v->y - std::sin(angle) * v->z;
        v->z = std::sin(angle) * v->y + std::cos(angle) * v->z;
    }

    typedef Vec3<float> Vec3f;
    typedef Vec3<int> Vec3i;
}
