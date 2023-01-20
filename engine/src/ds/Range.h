#pragma once

template<typename T>
struct Range
{
    Range () {}
    Range (T min, T max) : min(min), max(max) {}
    Range (T max) : min(0), max(max) {}

    T min = 0;
    T max = 0;
};

typedef Range<int> Rangei;
typedef Range<float> Rangef;
