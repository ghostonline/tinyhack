#pragma once

#include <climits>

class Random
{
    static const unsigned m = static_cast<unsigned>(1 << 31) - 1;
    static const int a = 1103515245;
    static const int c = 12345;
public:
    Random()
    : seed(0)
    {}

    Random(int seed)
    : seed(seed)
    {}

    void set_seed(int value) { seed = value; }

    int next()
    {
        seed = (a * seed + c) % m;
        return seed;
    }

    int next(int max)
    {
        return next() % max;
    }

    int next(int min, int max)
    {
        return (next() % (max - min)) + min;
    }

    float next_float()
    {
        return next() / static_cast<float>(INT_MAX);
    }

    bool next_bool()
    {
        return (next() & 0x10) > 0;
    }

private:
    int seed;
};
