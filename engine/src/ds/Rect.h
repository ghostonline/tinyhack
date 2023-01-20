#pragma once

#include <math/Math.h>

template<typename T>
struct Rect
{
    union
    {
        T x;
        T left;
    };
    union
    {
        T y;
        T top;
    };
    T right;
    T bottom;

    Rect();
    Rect(T x, T y, T width, T height)
    : x(x), y(y), right(x + width), bottom(y + height)
    {}

    void set(T x, T y, T width, T height);
    void offset(T dx, T dy);
    void explode(T delta) { explode(delta, delta); }
    void explode(T dx, T dy);
    bool contains(T x, T y) const;
    bool contains_inclusive(T x, T y) const;
    bool contains_exclusive(T x, T y) const;

    T center_x() const;
    T center_y() const;
    T width() const { return right - left; }
    T height() const { return bottom - top; }

    static Rect get_bounding_rect(const Rect& a, const Rect& b);
    static Rect get_intersecting_rect(const Rect& a, const Rect& b);
    static bool overlaps(const Rect& a, const Rect& b);
};

template<typename T>
inline void Rect<T>::set(T x, T y, T width, T height)
{
    this->left = x;
    this->top = y;
    this->right = x + width;
    this->bottom = y + height;
}

template<typename T>
inline void Rect<T>::offset(T dx, T dy)
{
    left += dx;
    top += dy;
    right += dx;
    bottom += dy;
}

template<typename T>
inline void Rect<T>::explode(T dx, T dy)
{
    left -= dx;
    right += dx;
    top -= dy;
    bottom += dy;
}

template<>
inline Rect<float>::Rect()
: left(0.0f), top(0.0f), right(0.0f), bottom(0.0f)
{}

template<>
inline Rect<int>::Rect()
: left(0), top(0), right(0), bottom(0)
{}

template<>
inline int Rect<int>::center_x() const { return (left + right) >> 1; }
template<>
inline int Rect<int>::center_y() const { return (top + bottom) >> 1; }
template<>
inline float Rect<float>::center_x() const { return (left + right) * 0.5f; }
template<>
inline float Rect<float>::center_y() const { return (top + bottom) * 0.5f; }

template<typename T>
inline bool Rect<T>::contains(T point_x, T point_y) const
{
    return left <= point_x && point_x< right && top <= point_y && point_y < bottom;
}

template<typename T>
inline bool Rect<T>::contains_exclusive(T point_x, T point_y) const
{
    return left < point_x && point_x < right && top < point_y && point_y < bottom;
}

template<typename T>
inline bool Rect<T>::contains_inclusive(T point_x, T point_y) const
{
    return left <= point_x && point_x <= right && top <= point_y && point_y <= bottom;
}

template<typename T>
inline Rect<T> Rect<T>::get_bounding_rect(const Rect<T>& a, const Rect<T>& b)
{
    Rect<T> bounding;
    bounding.left = math::min(a.left, b.left);
    bounding.top = math::min(a.top, b.top);
    bounding.right = math::max(a.right, b.right);
    bounding.bottom = math::max(a.bottom, b.bottom);
    return bounding;
}

template<typename T>
inline Rect<T> Rect<T>::get_intersecting_rect(const Rect<T>& a, const Rect<T>& b)
{
    Rect<T> intersect;
    intersect.left = math::max(a.left, b.left);
    intersect.top = math::max(a.top, b.top);
    intersect.right = math::min(a.right, b.right);
    intersect.bottom = math::min(a.bottom, b.bottom);
    return intersect;
}

template<typename T>
bool Rect<T>::overlaps(const Rect<T>& a, const Rect<T>& b)
{
    if (a.right <= b.left) return false; // a is left of b
    if (a.bottom <= b.top) return false; // a is above b
    if (a.left >= b.right) return false; // a is right of b
    if (a.top >= b.bottom) return false; // a is below b
    return true;
}

typedef Rect<float> Rectf;
typedef Rect<int> Recti;
