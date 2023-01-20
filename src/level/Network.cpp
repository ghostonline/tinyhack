#include "Network.h"

#include <memory>

void Network::reset(const Size2i& new_size)
{
    size = new_size;
    tiles.resize(new_size.width, new_size.height);
    memset(tiles.data(), 0, tiles.size_in_bytes());
    patrolling_enemies.clear();
}

const Tile* Network::get_tile_safe(const math::Vec2i& pos) const
{
    if (tiles.contains(pos.x, pos.y))
    {
        return &tiles.at(pos.x, pos.y);
    }
    else
    {
        static Tile empty_tile;
        empty_tile.type = TileType::Empty;
        return &empty_tile;
    }
}
