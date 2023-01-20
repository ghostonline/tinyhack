#include "World.h"
#include <Direction.h>

World::SubnetConnection World::get_subnet_connections(const math::Vec2i& pos) const
{
    auto* tile = network.get_tile(pos);
    T3D_ASSERT(tile->type == TileType::Connector);
    static const math::Vec2i connector_offsets[] = {
        math::Vec2i::AxisX,
        math::Vec2i::AxisY,
    };
    T3D_ASSERT(static_cast<int>(tile->connector().type) < 2); // Make sure to never exceed this array
    auto connector_offset = connector_offsets[static_cast<int>(tile->connector().type)];
    SubnetID a = network.get_tile(pos + connector_offset)->node().subnet_id;
    SubnetID b = network.get_tile(pos - connector_offset)->node().subnet_id;
    return {a, b};
}

bool World::is_subnet_separator(math::Vec2i pos) const
{
    auto connection = get_subnet_connections(pos);
    return connection.a != connection.b;
}

bool World::is_connected_to_subnet(math::Vec2i pos, SubnetID subnet) const
{
    auto connection = get_subnet_connections(pos);
    return connection.a == subnet || connection.b == subnet;
}

bool World::is_connected_to_visible_node(math::Vec2i pos) const
{
    bool connected = false;
    if (network.tiles.contains(pos.x, pos.y))
    {
        std::vector<math::Vec2i> test_sites;
        auto* tile = network.get_tile(pos);
        if (tile->type == TileType::Node)
        {
            test_sites.reserve(4);
            for (int dir = Direction::First; dir <= Direction::Last; ++dir)
            {
                auto delta = Direction::to_vec2i(dir);
                if (network.get_tile(pos + delta)->type == TileType::Connector)
                {
                    test_sites.emplace_back(pos + delta * 2);
                }
            }
        }
        else if (tile->type == TileType::Connector)
        {
            test_sites.reserve(2);
            if (tile->connector().type == ConnectorType::Horizontal)
            {
                test_sites.emplace_back(pos + math::Vec2i::AxisX);
                test_sites.emplace_back(pos - math::Vec2i::AxisX);
            }
            else
            {
                test_sites.emplace_back(pos + math::Vec2i::AxisY);
                test_sites.emplace_back(pos - math::Vec2i::AxisY);
            }
        }

        for (auto& site : test_sites)
        {
            if (get_visibility(site) == Visibility::Visible)
            {
                connected = true;
                break;
            }
        }
    }
    return connected;
}

void World::update_visibility_map()
{
    // Update all known nodes first
    for (int y = 0; y < network.size.height; ++y)
    {
        for (int x = 0; x < network.size.width; ++x)
        {
            math::Vec2i pos{x, y};
            auto* tile = network.get_tile(pos);
            if (tile->type == TileType::Node && known_subnets[tile->node().subnet_id])
            {
                visibility_map.at(x, y) = Visibility::Visible;
            }
        }
    }

    // Update all connections
    for (int y = 0; y < network.size.height; ++y)
    {
        for (int x = 0; x < network.size.width; ++x)
        {
            if (visibility_map.at(x, y) != Visibility::Visible && is_connected_to_visible_node({x, y}))
            {
                visibility_map.at(x, y) = Visibility::Detected;
            }
        }
    }
}
