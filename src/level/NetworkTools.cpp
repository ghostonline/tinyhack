#include "NetworkTools.h"

#include <Direction.h>
#include <RangeUtil.h>
#include <algorithm/PathFinder.h>
#include <diag/Assert.h>
#include <level/Network.h>

#include <deque>
#include <iterator>

void networktools::get_node_neighbours(const Network& network, const math::Vec2i& pos, std::vector<math::Vec2i>& results)
{
    T3D_ASSERT(network.get_tile(pos)->type == TileType::Node);
    results.clear();
    results.reserve(4);
    for (int dir = Direction::First; dir <= Direction::Last; ++dir)
    {
        auto delta = Direction::to_vec2i(dir);
        auto connector_pos = pos + delta;
        auto node_pos = connector_pos + delta;
        if (network.get_tile(connector_pos)->type == TileType::Connector)
        {
            results.push_back(node_pos);
        }
    }
}

void networktools::visit_nodes(const Network& network, const math::Vec2i& start_pos, NodeCallback callback)
{
    Array2<bool> visited_nodes(network.size.width, network.size.height);
    std::deque<VisitData> visitors;
    visited_nodes.at(start_pos.x, start_pos.y) = true;
    visitors.emplace_back(start_pos, 0);

    std::vector<math::Vec2i> neighbours_container;
    while(!visitors.empty())
    {
        auto visitor = visitors.front();
        visitors.pop_front();
        T3D_ASSERT(network.get_tile_safe(visitor.pos)->type == TileType::Node);
        CallbackResult result = callback(visitor);
        switch(result)
        {
        default:
            T3D_FAIL("Unhandled callback result");
        case CallbackResult::StopVisitor:
            break;
        case CallbackResult::StopAllVisitors:
            visitors.clear();
            break;
        case CallbackResult::Continue:
            get_node_neighbours(network, visitor.pos, neighbours_container);
            for (auto& node_pos : neighbours_container)
            {
                if (!visited_nodes.at(node_pos.x, node_pos.y))
                {
                    visited_nodes.at(node_pos.x, node_pos.y) = true;
                    visitors.emplace_back(node_pos, visitor.distance_from_start + 1);
                }
            }
            break;
        }
    }
}

struct NetworkWalker : public pathfinder::GraphWalker
{
    using NodeID = pathfinder::NodeID;
    using NodeList = pathfinder::NodeList;

    virtual NodeList get_neighbours(NodeID id) const
    {
        networktools::get_node_neighbours(*network, from_id(id), neighbours);
        NodeList result; result.reserve(neighbours.size());
        range::transform(neighbours, std::back_inserter(result), [this](math::Vec2i pos) { return to_id(pos); });
        return result;
    }

    virtual float get_cost(NodeID start, NodeID end) const
    {
        return 1.0f; // Constant cost
    }

    virtual float get_estimated_cost(NodeID start, NodeID end) const
    {
        return math::distance(from_id(start), from_id(end));
    }

    NodeID to_id(const math::Vec2i& pos) const
    {
        return static_cast<NodeID>(network->tiles.get_index(pos.x, pos.y));
    }

    math::Vec2i from_id(NodeID id) const
    {
        std::size_t x, y;
        network->tiles.get_pos(id, &x, &y);
        return {static_cast<int>(x), static_cast<int>(y)};
    }

    const Network* network = nullptr;

private:
    mutable std::vector<math::Vec2i> neighbours; // cache
};

std::vector<math::Vec2i> networktools::find_path(const Network& network, const math::Vec2i& start_pos, const math::Vec2i& end_pos)
{
    static NetworkWalker walker;
    walker.network = &network;
    pathfinder::NodeList path = pathfinder::find_path(walker, walker.to_id(start_pos), walker.to_id(end_pos));
    if (path.empty())
    {
        return {};
    }
    else
    {
        std::vector<math::Vec2i> results;
        results.reserve(path.size());
        range::transform(path, std::back_inserter(results), [](pathfinder::NodeID id) { return walker.from_id(id); });
        return results;
    }
}
