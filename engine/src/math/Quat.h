#pragma once

#include "Math_misc.h"
#include "Vec3.h"

#include <cmath>

namespace math
{

    template<typename T>
    struct Quat
    {
        typedef Quat<T> QuatType;
        typedef Vec3<T> ImaginaryType;

        T s;
        ImaginaryType v;

        Quat() : s(0), v(0, 0, 0) {}
        Quat(T s, const ImaginaryType& v) : s(s), v(v) {}

        QuatType& operator+=(const QuatType& rhs)
        {
            s += rhs.s;
            v += rhs.v;
            return *this;
        }

        QuatType& operator-=(const QuatType& rhs)
        {
            s -= rhs.s;
            v -= rhs.v;
            return *this;
        }

        QuatType& operator*=(const QuatType& rhs)
        {
            float new_s = s*rhs.s - dot(v, rhs.v);
            ImaginaryType new_v = rhs.v*s + v*rhs.s + cross(v, rhs.v);
            s = new_s;
            v = new_v;
            return *this;
        }

        QuatType& operator*=(T rhs)
        {
            s *= rhs;
            v *= rhs;
            return *this;
        }

        QuatType operator+(const QuatType& rhs) const
        {
            QuatType copy = *this;
            copy += rhs;
            return copy;
        }

        QuatType operator-(const QuatType& rhs) const
        {
            QuatType copy = *this;
            copy -= rhs;
            return copy;
        }

        QuatType operator*(const QuatType& rhs) const
        {
            QuatType copy = *this;
            copy *= rhs;
            return copy;
        }

        QuatType operator*(T rhs) const
        {
            QuatType copy = *this;
            copy *= rhs;
            return copy;
        }
    };

    template<typename T>
    inline T magnitude(const Quat<T>& quat)
    {
        float scalar = quat.s*quat.s;
        float imaginary = length2(quat.v);
        return std::sqrt(scalar+imaginary);
    }

    template<typename T>
    inline void normalize(Quat<T>* quat)
    {
        T mag = magnitude(*quat);
        if (mag!=0)
        {
            (*quat) *= 1/mag;
        }
    }

    template<typename T>
    inline Quat<T> normalized(const Quat<T>& quat)
    {
        Quat<T> copy = quat;
        normalize(&copy);
        return copy;
    }

    template<typename T>
    inline Quat<T> to_unit_norm(const Quat<T>& quat)
    {
        Vec3<T> v = normalized(quat.v);
        T s = std::cos(quat.s / 2);
        v = v * std::sin(quat.s / 2);
        return {s, v};
    }

    template<typename T>
    inline Quat<T> conjugate(const Quat<T>& quat)
    {
        return {quat.s, -quat.v};
    }

    template<typename T>
    inline Quat<T> inversed(const Quat<T>& quat)
    {
        return {quat.s, -quat.v};
    }

    template<typename T>
    inline Vec3<T> apply(const Quat<T>& quat, const Vec3<T>& vec)
    {
        const Quat<T> p(0, vec);
        const Quat<T> inverse = inversed(quat);
        const Quat<T> rotated = quat * p * inverse;
        return rotated.v;
    }

    template<typename T>
    inline Quat<T> get_rotation(T angle, const Vec3<T>& axis)
    {
        return to_unit_norm(Quat<T>{angle, axis});
    }

    template<typename T>
    inline Quat<T> get_rotation(const Vec3<T>& start, const Vec3<T>& dest)
    {
        auto norm_start = normalized(start);
        auto norm_dest = normalized(dest);
        auto cos_theta = dot(norm_start, norm_dest);
        Vec3<T> axis;

        // opposite direction
        if (cos_theta < -1 + APPROXIMATE_ZERO)
        {
            axis = cross(Vec3<T>::AxisX, start);
            if (length2(axis) < APPROXIMATE_ZERO) // perpendicular
            {
                axis = cross(Vec3<T>::AxisY, start);
            }
            normalize(&axis);
            return get_rotation(PI, axis);
        }
        else if (std::abs(1 - cos_theta) < APPROXIMATE_ZERO)
        {
            return get_rotation<T>(0, Vec3<T>::AxisX);
        }
        else
        {
            axis = cross(start, dest);
            return get_rotation(std::acos(cos_theta), axis);
        }
    }

    typedef Quat<float> Quatf;

}
