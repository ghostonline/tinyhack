#pragma once

namespace math
{
    const float PI = 3.1415926535897932384626433832795028841f;
    const float PI2 = PI * 2;
    const float APPROXIMATE_ZERO = 0.00001f;

    inline float deg2rad(float deg) { return (PI / 180.0f) * deg; }

    template<typename T>
    T squared(T num) { return num * num; }

    template<typename T>
    T min(T a, T b) { return a < b ? a : b; }

    template<typename T>
    T max(T a, T b) { return a > b ? a : b; }

    template<typename T>
    T clamp(T num, T min, T max) { return math::max<T>(min, math::min<T>(num, max)); }
    inline float clamp01(float num) { return clamp(num, 0.0f, 1.0f); }
    inline double clamp01(double num) { return clamp(num, 0.0, 1.0); }

    inline int sign(int v) { return v > 0 ? 1 : -1; }
    inline float sign(float v) { return v > 0 ? 1.0f : -1.0f; }

    template<typename T>
    inline T lerp(const T& a, const T& b, float factor) { return a + (b - a) * factor; }

    inline int round_to_int(float v) { return static_cast<int>(v + 0.5f); }

    template<typename T>
    inline T abs(T v) { return v < 0 ? v * -1 : v; }

    // Source: https://jeffreystedfast.blogspot.com/2008/06/calculating-nearest-power-of-2.html
    inline unsigned nearest_po2(unsigned num)
    {
        static_assert(sizeof(num) == 4, "Logic only works with 32bit integers");
        unsigned n = num > 0 ? num - 1 : 0;

        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        n++;

        return n;
    }

    inline int nearest_po2(int num)
    {
        if (num < 0)
        {
            unsigned n = static_cast<unsigned>(num * -1);
            return nearest_po2(n) * -1;
        }
        else
        {
            unsigned n = static_cast<unsigned>(num);
            return nearest_po2(n);
        }
    }
}
