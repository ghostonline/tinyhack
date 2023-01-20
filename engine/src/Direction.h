#pragma once

#include <math/Vec2.h>

struct Direction
{
    enum Type
    {
        East,
        North,
        West,
        South,

        Right = East,
        Up = North,
        Left = West,
        Down = South,

        First = East,
        Last = Down,
    };

    static const int Count = (Last - First) + 1;

    inline static const math::Vec2i& to_vec2i(int index) { return to_vec2i(static_cast<Type>(index)); }
    static const math::Vec2i& to_vec2i(Direction::Type direction);
    static const Direction::Type& get_opposite(Direction::Type direction);
};
