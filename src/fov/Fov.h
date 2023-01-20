#pragma once

#include <math/Vec2.h>

#include <functional>
#include <bitset>

namespace fov
{
    using OctantSet = std::bitset<8>;

    // Taken and converted from http://www.adammil.net/blog/v125_Roguelike_Vision_Algorithms.html#mine
    class MyVisibility
    {
    public:
        using BlockLightFunc = std::function<bool(int, int)>;
        using SetVisibleFunc = std::function<void(int, int)>;
        using GetDistanceFunc = std::function<int(int, int)>;
        using LevelPoint = math::Vec2i;
        using uint = unsigned;

    private:
        struct Slope // represents the slope Y/X as a rational number
        {
            Slope(uint y, uint x) : X(x), Y(y) {}

            bool Greater(uint y, uint x) const { return Y*x > X*y; } // this > y/x
            bool GreaterOrEqual(uint y, uint x) const { return Y*x >= X*y; } // this >= y/x
            bool Less(uint y, uint x) const { return Y*x < X*y; } // this < y/x
                                                                  //public bool LessOrEqual(uint y, uint x) { return Y*x <= X*y; } // this <= y/x

            uint X, Y;
        };

    public:
        /// <param name="blocksLight">A function that accepts the X and Y coordinates of a tile and determines whether the
        /// given tile blocks the passage of light. The function must be able to accept coordinates that are out of bounds.
        /// </param>
        /// <param name="setVisible">A function that sets a tile to be visible, given its X and Y coordinates. The function
        /// must ignore coordinates that are out of bounds.
        /// </param>
        /// <param name="getDistance">A function that takes the X and Y coordinate of a point where X >= 0,
        /// Y >= 0, and X >= Y, and returns the distance from the point to the origin (0,0).
        /// </param>
        MyVisibility(BlockLightFunc blocksLight, SetVisibleFunc setVisible, GetDistanceFunc getDistance);
        void Compute(LevelPoint origin, OctantSet octants_to_compute, int rangeLimit);

    private:
        void Compute(uint octant, LevelPoint origin, int rangeLimit, uint x, Slope top, Slope bottom);

        // NOTE: the code duplication between BlocksLight and SetVisible is for performance. don't refactor the octant
        // translation out unless you don't mind an 18% drop in speed
        inline bool BlocksLight(uint x, uint y, uint octant, const LevelPoint& origin)
        {
            uint nx = origin.x, ny = origin.y;
            translate_octant(octant, nx, ny, x, y);
            return _blocksLight((int)nx, (int)ny);
        }

        inline void SetVisible(uint x, uint y, uint octant, const LevelPoint& origin)
        {
            uint nx = origin.x, ny = origin.y;
            translate_octant(octant, nx, ny, x, y);
            _setVisible((int)nx, (int)ny);
        }

        inline static void translate_octant(uint octant, uint& nx, uint& ny, uint x, uint y)
        {
            switch(octant)
            {
            case 0: nx += x; ny -= y; break;
            case 1: nx += y; ny -= x; break;
            case 2: nx -= y; ny -= x; break;
            case 3: nx -= x; ny -= y; break;
            case 4: nx -= x; ny += y; break;
            case 5: nx -= y; ny += x; break;
            case 6: nx += y; ny += x; break;
            case 7: nx += x; ny += y; break;
            }
        }

        BlockLightFunc _blocksLight;
        GetDistanceFunc GetDistance;
        SetVisibleFunc _setVisible;
    };
}
