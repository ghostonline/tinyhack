#include <Direction.h>

const math::Vec2i& Direction::to_vec2i(Direction::Type direction)
{
    static const math::Vec2i deltas[] =
    {
        math::Vec2i{ 1,  0},
        math::Vec2i{ 0, -1},
        math::Vec2i{-1,  0},
        math::Vec2i{ 0,  1},
    };
    return deltas[direction];
}

const Direction::Type& Direction::get_opposite(Direction::Type direction)
{
    static const Direction::Type opposites[] =
    {
        Direction::West,
        Direction::South,
        Direction::East,
        Direction::North,
    };
    return opposites[direction];
}
