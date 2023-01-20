#pragma once

#include <math/Vec2.h>
#include <functional>
#include <vector>

struct Network;

namespace networktools
{
    enum CallbackResult
    {
        Continue,
        StopVisitor,
        StopAllVisitors,
    };

    struct VisitData
    {
        VisitData(math::Vec2i pos, int distance_from_start)
            : pos(pos), distance_from_start(distance_from_start)
        {}

        math::Vec2i pos;
        int distance_from_start = 0;
    };

    using NodeCallback = std::function<CallbackResult(const VisitData&)>;

    std::vector<math::Vec2i> get_node_neighbours(const Network& network, const math::Vec2i& pos);
    void get_node_neighbours(const Network& network, const math::Vec2i& pos, std::vector<math::Vec2i>& results);
    void visit_nodes(const Network& network, const math::Vec2i& start_pos, NodeCallback callback);
    std::vector<math::Vec2i> find_path(const Network& network, const math::Vec2i& start_pos, const math::Vec2i& end_pos);
}

inline std::vector<math::Vec2i> networktools::get_node_neighbours(const Network& network, const math::Vec2i& pos)
{
    std::vector<math::Vec2i> neighbours;
    get_node_neighbours(network, pos, neighbours);
    return neighbours;
}
