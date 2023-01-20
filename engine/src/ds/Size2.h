#pragma once

template<typename T>
struct Size2
{
    Size2 () {}
    Size2 (T width, T height) : width(width), height(height) {}
    Size2 (T side) : width(side), height(side) {}

    T width = 0;
    T height = 0;

    bool contains(T x, T y) const { return 0 <= x && x < width && 0 <= y && y < height; }
    bool contains_inclusive(T x, T y) const { return 0 <= x && x <= width && 0 <= y && y <= height; }
    bool contains_exclusive(T x, T y) const { return 0 < x && x < width && 0 < y && y < height; }
};

template<typename T>
inline bool operator==(const Size2<T>& lhs, const Size2<T>& rhs)
{
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

template<typename T>
inline bool operator!=(const Size2<T>& lhs, const Size2<T>& rhs)
{
    return !(lhs == rhs);
}

typedef Size2<int> Size2i;
typedef Size2<float> Size2f;
