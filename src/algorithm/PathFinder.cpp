#include "PathFinder.h"

#include <RangeUtil.h>
#include <game/World.h>

#include <algorithm>
#include <functional>
#include <iterator>
#include <queue>
#include <unordered_set>

static const float InfiniteCost = 1000000000000.0f;

namespace pathfinder
{

struct Node
{
    Node(float g = InfiniteCost, float f = InfiniteCost) : g(g), f(f) {}

    float g; // cost from start to this node
    float f; // cost from start to goal through this node
};

using StoredType = std::pair<NodeID, Node>;

NodeList find_path(const GraphWalker& world, const NodeID& start, const NodeID& end)
{
    std::unordered_set<NodeID> closed_set;
    std::unordered_map<NodeID, Node> open_set;
    open_set.insert({start, {0.0f, world.get_estimated_cost(start, end)}});

    NodeList path;
    std::unordered_map<NodeID, NodeID> found_path;
    while (!open_set.empty())
    {
        auto current = range::min_element(open_set, [](const StoredType& lhs, const StoredType& rhs) { return lhs.second.g < rhs.second.g; });
        auto current_pos = current->first;
        auto current_data = current->second;
        if (current_pos == end)
        {
            path.push_back(current_pos);
            while (found_path.find(current_pos) != std::end(found_path))
            {
                current_pos = found_path[current_pos];
                path.push_back(current_pos);
            }
            break;
        }

        open_set.erase(current);
        closed_set.emplace(current_pos);

        for (auto& neighbour_data : world.get_neighbours(current_pos))
        {
            if (closed_set.find(neighbour_data) != std::end(closed_set))
            {
                continue;
            }

            float tentative_g = current_data.g + world.get_cost(current_pos, neighbour_data);

            auto stored_in_open = open_set.find(neighbour_data);
            if (stored_in_open == std::end(open_set))
            {
                open_set.insert({neighbour_data, {}});
                stored_in_open = open_set.find(neighbour_data);
                T3D_ASSERT(stored_in_open != std::end(open_set));
            }
            else if (stored_in_open->second.g <= tentative_g)
            {
                continue;
            }

            found_path[neighbour_data] = current_pos;
            stored_in_open->second.g = tentative_g;
            stored_in_open->second.f = tentative_g + world.get_estimated_cost(neighbour_data, end);
        }
    }

    std::vector<NodeID> fixed_path;
    fixed_path.assign(path.rbegin(), path.rend());
    return fixed_path;
}

}
