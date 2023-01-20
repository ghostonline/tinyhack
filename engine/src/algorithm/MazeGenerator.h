#pragma once

#include <Direction.h>
#include <ds/Array2.h>
#include <ds/Size2.h>

class Random;

namespace maze
{
    struct Cell
    {
        unsigned distance_from_origin = 0;
        unsigned x = 0;
        unsigned y = 0;

        bool exits[Direction::Count] = {};
        inline bool has_exit()
        {
            for (int i = Direction::First; i <= Direction::Last; ++i)
            {
                if (exits[i])
                {
                    return true;
                }
            }

            return false;
        }
    };

    Array2<Cell> generate(const Array2<bool>& map, Random& rng, math::Vec2i start);
    inline Array2<Cell> generate(const Size2i& size, Random& rng, math::Vec2i start) { return generate(Array2<bool>(size.width, size.height, false), rng, start); }
}
