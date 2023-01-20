#pragma once

#include <ds/Array2.h>
#include <ds/Size2.h>
#include <math/Vec2.h>

enum class TileType
{
    Empty,
    Node,
    Connector,
};

enum class NodeType
{
    Normal,
    Entrance,
    Exit,
    DataStore,
};

using SubnetID = std::size_t;
static const SubnetID SubnetIDNull = 0xFFFFFFFF;

struct NodeData
{
    SubnetID subnet_id;
    NodeType type;
};

enum class ConnectorType
{
    Horizontal,
    Vertical,
};

struct ConnectorData
{
    ConnectorType type;
};

struct Tile
{
    TileType type = TileType::Empty;

    NodeData& node() { T3D_ASSERT(type == TileType::Node); return data.node; }
    const NodeData& node() const { T3D_ASSERT(type == TileType::Node); return data.node; }
    ConnectorData& connector() { T3D_ASSERT(type == TileType::Connector); return data.connector; }
    const ConnectorData& connector() const { T3D_ASSERT(type == TileType::Connector); return data.connector; }

private:
    union
    {
        NodeData node;
        ConnectorData connector;
    } data;
};

struct PatrollingEnemy
{
    math::Vec2i point_a;
    math::Vec2i point_b;
};

struct Network
{
    void reset(const Size2i& new_size);
    Tile* get_tile(const math::Vec2i& pos) { return &tiles.at(pos.x, pos.y); }
    const Tile* get_tile(const math::Vec2i& pos) const { return &tiles.at(pos.x, pos.y); }
    Tile* get_tile_safe(const math::Vec2i& pos) { return const_cast<Tile*>(const_cast<const Network*>(this)->get_tile_safe(pos)); } // Cast away const so we can share implementation
    const Tile* get_tile_safe(const math::Vec2i& pos) const;

    Size2i size;
    Array2<Tile> tiles;
    unsigned subnet_count = 0;
    math::Vec2i entrance;
    math::Vec2i exit;
    std::vector<PatrollingEnemy> patrolling_enemies;
};
