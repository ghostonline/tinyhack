#pragma once

#include <math/Vec2.h>

#include <vector>

struct World;

namespace pathfinder
{
    using NodeID = unsigned;
    using NodeList = std::vector<NodeID>;

    struct GraphWalker
    {
        virtual NodeList get_neighbours(NodeID id) const = 0;
        virtual float get_cost(NodeID start, NodeID end) const = 0;
        virtual float get_estimated_cost(NodeID start, NodeID end) const = 0;
    };

    NodeList find_path(const GraphWalker& world, const NodeID& start, const NodeID& end);
}
