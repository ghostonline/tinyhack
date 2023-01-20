#include "MazeGenerator.h"

#include <Random.h>
#include <RandomUtil.h>
#include <RangeUtil.h>
#include <math/Vec2.h>

#include <iterator>
#include <vector>

namespace
{

    inline bool is_visited(const maze::Cell& cell)
    {
        return cell.exits[Direction::South] || cell.exits[Direction::North] || cell.exits[Direction::West] || cell.exits[Direction::East];
    }

    bool try_add(std::vector<math::Vec2i>& list, math::Vec2i pos, const Array2<maze::Cell>& layout, const Array2<bool>& collision_layout)
    {
        if (!layout.contains(pos.x, pos.y))
        {
            return false;
        }

        if (collision_layout.at(pos.x, pos.y))
        {
            return false;
        }

        if (is_visited(layout.at(pos.x, pos.y)))
        {
            return false;
        }

        list.push_back(pos);
        return true;
    }

}

Array2<maze::Cell> maze::generate(const Array2<bool>& map, Random& rng, math::Vec2i start)
{
    Array2<Cell> layout(map.width(), map.height());

    std::vector<math::Vec2i> open_nodes;
    open_nodes.emplace_back(start);

    while(!open_nodes.empty())
    {
        auto pos = open_nodes.back();
        open_nodes.pop_back();
        auto& tile = layout.at(pos.x, pos.y);
        tile.x = pos.x;
        tile.y = pos.y;
        std::vector<math::Vec2i> neighbours;
        if (!tile.exits[Direction::North] && try_add(neighbours, pos + -math::Vec2i::AxisY, layout, map))
        {
            tile.exits[Direction::North] = true;
            auto& other_tile = layout.at(pos.x, pos.y - 1);
            other_tile.distance_from_origin = tile.distance_from_origin + 1;
            other_tile.exits[Direction::South] = true;
        }
        if (!tile.exits[Direction::South] && try_add(neighbours, pos + math::Vec2i::AxisY, layout, map))
        {
            tile.exits[Direction::South] = true;
            auto& other_tile = layout.at(pos.x, pos.y + 1);
            other_tile.distance_from_origin = tile.distance_from_origin + 1;
            other_tile.exits[Direction::North] = true;
        }
        if (!tile.exits[Direction::West] && try_add(neighbours, pos + -math::Vec2i::AxisX, layout, map))
        {
            tile.exits[Direction::West] = true;
            auto& other_tile = layout.at(pos.x - 1, pos.y);
            other_tile.distance_from_origin = tile.distance_from_origin + 1;
            other_tile.exits[Direction::East] = true;
        }
        if (!tile.exits[Direction::East] && try_add(neighbours, pos + math::Vec2i::AxisX, layout, map))
        {
            tile.exits[Direction::East] = true;
            auto& other_tile = layout.at(pos.x + 1, pos.y);
            other_tile.distance_from_origin = tile.distance_from_origin + 1;
            other_tile.exits[Direction::West] = true;
        }

        util::shuffle(neighbours, rng);
        range::copy(neighbours, std::back_inserter(open_nodes));
    }
    return layout;
}
