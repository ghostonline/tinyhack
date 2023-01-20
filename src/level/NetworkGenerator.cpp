#include "NetworkGenerator.h"

#include "Network.h"
#include "NetworkTools.h"

#include <Random.h>
#include <RandomUtil.h>
#include <RangeUtil.h>
#include <Util.h>
#include <algorithm/MazeGenerator.h>
#include <algorithm/PathFinder.h>
#include <ds/Range.h>
#include <ds/Rect.h>
#include <math/Math_misc.h>

#include <array>
#include <cstddef>
#include <deque>

struct ExitSpec
{
    ExitSpec() = default;
    ExitSpec(Direction::Type dir, ConnectorType connector_type)
        : dir(dir), pos_offset(Direction::to_vec2i(dir)), connector_type(connector_type)
    {}

    Direction::Type dir;
    math::Vec2i pos_offset;
    ConnectorType connector_type = ConnectorType::Horizontal;
};

struct MazeWalker : public pathfinder::GraphWalker
{
    using Cell = maze::Cell;
    using NodeList = pathfinder::NodeList;
    using NodeID = pathfinder::NodeID;
    using Maze = Array2<maze::Cell>;

    Maze* maze_ptr = nullptr;

    inline NodeID get_adjacent(NodeID current, Direction::Type dir) const
    {
        return get_adjacent(maze_ptr->at(current), dir);
    }

    inline NodeID get_cell_id(const Cell& cell) const
    {
        return static_cast<NodeID>(maze_ptr->get_index(cell.x, cell.y));
    }

    inline Cell* get_cell(NodeID id)
    {
        return &maze_ptr->at(id);
    }

    inline const Cell* get_cell(NodeID id) const
    {
        return &maze_ptr->at(id);
    }

    inline NodeList find_path(NodeID start, NodeID end) const
    {
        return pathfinder::find_path(*this, start, end);
    }

    bool try_get_adjacent(const Cell& current, Direction::Type dir, NodeID* id) const
    {
        math::Vec2i delta = Direction::to_vec2i(dir);
        Maze::size_type adj_x = current.x + delta.x;
        Maze::size_type adj_y = current.y + delta.y;
        if (maze_ptr->contains(adj_x, adj_y))
        {
            *id = static_cast<NodeID>(maze_ptr->get_index(adj_x, adj_y));
            return true;
        }
        else
        {
            return false;
        }
    }

    NodeID get_adjacent(const Cell& current, Direction::Type dir) const
    {
        math::Vec2i delta = Direction::to_vec2i(dir);
        Maze::size_type adj_x = current.x + delta.x;
        Maze::size_type adj_y = current.y + delta.y;
        return static_cast<NodeID>(maze_ptr->get_index(adj_x, adj_y));
    }

    virtual NodeList get_neighbours(NodeID id) const override
    {
        const auto* cell = get_cell(id);
        NodeList list;
        list.reserve(Direction::Count);
        for (int exit_index = 0; exit_index < Direction::Count; ++exit_index)
        {
            if (cell->exits[exit_index])
            {
                auto dir = static_cast<Direction::Type>(exit_index);
                list.push_back(get_adjacent(*cell, dir));
            }
        }
        return list;
    }

    virtual float get_cost(NodeID start, NodeID end) const override
    {
        return 1; // Constant cost
    }

    virtual float get_estimated_cost(NodeID start, NodeID end) const override
    {
        Maze::size_type start_x, start_y;
        maze_ptr->get_pos(start, &start_x, &start_y);
        Maze::size_type end_x, end_y;
        maze_ptr->get_pos(end, &end_x, &end_y);
        int distance =
            math::squared(static_cast<int>(end_x) - static_cast<int>(start_x)) +
            math::squared(static_cast<int>(end_y) - static_cast<int>(start_y));
        return static_cast<float>(distance);
    }
};

std::vector<math::Vec2i> find_nodes_in_range(math::Vec2i start, int walk_range, Network* network)
{
    std::vector<math::Vec2i> nodes;
    networktools::visit_nodes(*network, start, [&nodes, walk_range](const networktools::VisitData& data)
    {
        if (data.distance_from_start == walk_range)
        {
            nodes.push_back(data.pos);
            return networktools::CallbackResult::StopVisitor;
        }
        else
        {
            return networktools::CallbackResult::Continue;
        }
    });
    return nodes;
}

void networkgenerator::generate(int seed, Network* network)
{
    static const std::size_t min_distance_before_creating_loop = 9;
    static const Size2i maze_size{15, 7};
    static const int crawler_spawn_life = 5;
    static const Rangei data_stores_in_network = {3, 8};
    static const unsigned num_patrols = 2;
    static const unsigned patrol_length = 4;

    const std::array<ExitSpec, 4> exits =
    {
        ExitSpec{ Direction::East,  ConnectorType::Horizontal },
        ExitSpec{ Direction::West,  ConnectorType::Horizontal },
        ExitSpec{ Direction::North, ConnectorType::Vertical },
        ExitSpec{ Direction::South, ConnectorType::Vertical },
    };

    Random rng(seed);

    // Create arbitrary holes in network mesh
    Array2<bool> collision_map(maze_size.width, maze_size.height, false);
    std::vector<math::Vec2i> hits;
    bool horizontal_axis = rng.next_bool();
    bool vertical_axis = !horizontal_axis || rng.next_bool(); // Make sure there is always one continuous axis to walk through
    Recti block_site {
        0, 0,
        horizontal_axis ? maze_size.width / 2 : maze_size.width,
        vertical_axis ? maze_size.height / 2 : maze_size.height
    };
    int block_count = block_site.width() * block_site.height() / 10;
    for (int block_index = 0; block_index < block_count; ++block_index)
    {
        unsigned neighbours = 0;
        math::Vec2i block;
        do
        {
            // Place block within the designated area (but at the edges)
            block.set(
                rng.next(1, block_site.width() - 1),
                rng.next(1, block_site.height() - 1)
            );

            neighbours = 0;
            for (auto& existing_hit : hits)
            {
                if (math::distance2(existing_hit, block) <= 2)
                {
                    ++neighbours;
                }
            }
        }
        while(neighbours > 1);

        hits.emplace_back(block);
    }

    if (horizontal_axis)
    {
        // Walking backwards so we can easily add new hits
        for (int hit_index = static_cast<int>(hits.size()) - 1; hit_index >= 0; --hit_index)
        {
            auto mirror_hit = hits[hit_index];
            mirror_hit.x = maze_size.width - mirror_hit.x - 1;
            hits.push_back(mirror_hit);
        }
    }

    if (vertical_axis)
    {
        // Walking backwards so we can easily add new hits
        for (int hit_index = static_cast<int>(hits.size()) - 1; hit_index >= 0; --hit_index)
        {
            auto mirror_hit = hits[hit_index];
            mirror_hit.y = maze_size.height - mirror_hit.y - 1;
            hits.push_back(mirror_hit);
        }
    }

    for (const auto& hit : hits)
    {
        collision_map.at(hit.x, hit.y) = true;
    }

    // Always start generating maze from the center, since those will never be blocked
    math::Vec2i maze_start{
        maze_size.width / 2,
        maze_size.height / 2,
    };
    auto maze = maze::generate(collision_map, rng, maze_start);
    MazeWalker maze_walker;
    maze_walker.maze_ptr = &maze;

    // Add loops to single nodes
    for (std::size_t y = 0; y < maze.height(); ++y)
    {
        for (std::size_t x = 0; x < maze.width(); ++x)
        {
            auto& cell = maze.at(x, y);
            int num_exits = cell.exits[Direction::North]
                + cell.exits[Direction::South]
                + cell.exits[Direction::East]
                + cell.exits[Direction::West];

            if (num_exits == 1)
            {
                auto cell_id = maze_walker.get_cell_id(cell);

                maze::Cell* furthest_cell = nullptr;
                std::size_t furthest_distance = min_distance_before_creating_loop;
                Direction::Type furthest_direction = Direction::North;
                for (std::size_t exit_index = 0; exit_index < exits.size(); ++exit_index)
                {
                    auto dir = static_cast<Direction::Type>(exit_index);
                    pathfinder::NodeID adjacent_id = 0;
                    if (!cell.exits[exit_index] && maze_walker.try_get_adjacent(cell, dir, &adjacent_id))
                    {
                        auto path = maze_walker.find_path(cell_id, adjacent_id);
                        if (!path.empty() && furthest_distance < path.size())
                        {
                            furthest_distance = path.size();
                            furthest_cell = maze_walker.get_cell(adjacent_id);
                            furthest_direction = dir;
                        }
                    }
                }

                if (furthest_cell)
                {
                    auto opposite_dir = Direction::get_opposite(furthest_direction);
                    furthest_cell->exits[opposite_dir] = true;
                    cell.exits[furthest_direction] = true;
                }
            }
        }
    }

    Array2<SubnetID> subnets(maze_size.width, maze_size.height, SubnetIDNull);
    struct Crawler
    {
        unsigned x = 0;
        unsigned y = 0;
        SubnetID previous_subnet_id = SubnetIDNull;
        SubnetID subnet_id = SubnetIDNull;
        int life = 0;
    };

    std::vector<Crawler> crawlers;
    std::vector<bool> subnet_usage;
    for (auto& cell : maze)
    {
        if (cell.has_exit())
        {
            crawlers.push_back({});
            auto& crawler = crawlers.back();
            crawler.subnet_id = subnet_usage.size();
            subnet_usage.push_back(true);
            crawler.life = crawler_spawn_life;
            crawler.x = cell.x;
            crawler.y = cell.y;
            subnets.at(crawler.x, crawler.y) = crawler.subnet_id;
            break;
        }
    }

    while (!crawlers.empty())
    {
        static const int crawler_index = 0;
        auto crawler = crawlers[crawler_index];
        crawlers[crawler_index] = crawlers.back();
        crawlers.pop_back();

        if (crawler.life == 0)
        {
            // Rebirth crawler with a new subnet
            crawler.previous_subnet_id = crawler.subnet_id;
            crawler.subnet_id = subnet_usage.size();
            subnet_usage.push_back(true);
            crawler.life = crawler_spawn_life;
            subnets.at(crawler.x, crawler.y) = crawler.subnet_id;
        }

        maze::Cell& my_cell = maze.at(crawler.x, crawler.y);
        std::vector<math::Vec2i> options;
        for (std::size_t exit_index = 0; exit_index < exits.size(); ++exit_index)
        {
            if (my_cell.exits[exit_index])
            {
                auto dir = static_cast<Direction::Type>(exit_index);
                auto delta = Direction::to_vec2i(dir);
                if (subnets.at(crawler.x + delta.x, crawler.y + delta.y) == SubnetIDNull)
                {
                    math::Vec2i option(crawler.x, crawler.y);
                    option += delta;
                    options.push_back(option);
                }
            }
        }

        if (options.empty() && crawler.life == crawler_spawn_life)
        {
            // If brand new crawler without any options, just revert to its previous subnet, to prevent single node subnets
            subnets.at(crawler.x, crawler.y) = crawler.previous_subnet_id;
            subnet_usage[crawler.subnet_id] = false;
        }
        else
        {
            for (auto& option : options)
            {
                crawlers.push_back({});
                auto& child = crawlers.back();
                child.subnet_id = crawler.subnet_id;
                child.life = crawler.life - 1;
                child.x = option.x;
                child.y = option.y;
                subnets.at(child.x, child.y) = child.subnet_id;
            }
        }
    }

    {
        std::vector<SubnetID> rewire(subnet_usage.size(), SubnetIDNull);
        std::deque<SubnetID> empty_subnets;
        for (SubnetID id = 0; id < subnet_usage.size(); ++id)
        {
            if (subnet_usage[id])
            {
                if (empty_subnets.empty())
                {
                    rewire[id] = id;
                }
                else
                {
                    rewire[id] = empty_subnets.front();
                    empty_subnets.pop_front();
                    empty_subnets.push_back(id);
                }
            }
            else
            {
                rewire[id] = SubnetIDNull;
                empty_subnets.push_back(id);
            }
        }

        subnet_usage.clear();
        for (std::size_t y = 0; y < maze.height(); ++y)
        {
            for (std::size_t x = 0; x < maze.width(); ++x)
            {
                SubnetID current = subnets.at(x, y);
                if (current == SubnetIDNull)
                {
                    continue;
                }

                SubnetID rewired = rewire[current];
                subnets.at(x, y) = rewired;
                if (subnet_usage.size() <= rewired)
                {
                    subnet_usage.resize(rewired + 1);
                }
                subnet_usage[rewired] = true;
            }
        }

#if ASSERTS_ENABLED
        for (SubnetID id = 0; id < subnet_usage.size(); ++id)
        {
            T3D_ASSERT(subnet_usage[id]); // All subnet numbers are used at least once
        }
#endif
    }

    // Connect nodes within the same subnet better
    auto same_subnet = [&](const maze::Cell& cell, Direction::Type dir)
    {
        auto adj_id = maze_walker.get_adjacent(cell, dir);
        auto adj_cell = maze_walker.get_cell(adj_id);
        return subnets.at(cell.x, cell.y) == subnets.at(adj_cell->x, adj_cell->y);
    };

    for (std::size_t y = 0; y < maze.height(); ++y)
    {
        for (std::size_t x = 0; x < maze.width(); ++x)
        {
            auto& cell = maze.at(x, y);
            int num_exits = cell.exits[Direction::North]
                + cell.exits[Direction::South]
                + cell.exits[Direction::East]
                + cell.exits[Direction::West];

            if (num_exits == 1)
            {
                auto cell_id = maze_walker.get_cell_id(cell);

                maze::Cell* furthest_cell = nullptr;
                std::size_t furthest_distance = 0;
                Direction::Type furthest_direction = Direction::North;
                for (std::size_t exit_index = 0; exit_index < exits.size(); ++exit_index)
                {
                    auto dir = static_cast<Direction::Type>(exit_index);
                    pathfinder::NodeID adjacent_id = 0;
                    if (!cell.exits[exit_index] && maze_walker.try_get_adjacent(cell, dir, &adjacent_id) && same_subnet(cell, dir))
                    {
                        auto path = maze_walker.find_path(cell_id, adjacent_id);
                        if (!path.empty() && furthest_distance < path.size())
                        {
                            furthest_distance = path.size();
                            furthest_cell = maze_walker.get_cell(adjacent_id);
                            furthest_direction = dir;
                        }
                    }
                }

                if (furthest_cell)
                {
                    auto opposite_dir = Direction::get_opposite(furthest_direction);
                    furthest_cell->exits[opposite_dir] = true;
                    cell.exits[furthest_direction] = true;
                }
            }
        }
    }

    // Determine entrance and exit nodes
    maze::Cell* level_entrance = nullptr;
    maze::Cell* level_exit = nullptr;
    do
    {
        const int maze_size_int = static_cast<int>(maze.size());
        do
        {
            auto cell = maze.data() + rng.next(maze_size_int);
            if (cell->has_exit())
            {
                level_entrance = cell;
            }
        } while(!level_entrance);

        do
        {
            auto cell = maze.data() + rng.next(maze_size_int);
            if (cell->has_exit())
            {
                level_exit = cell;
            }
        } while(!level_exit);
    } while(level_entrance == level_exit);

    // Render out the actual nodes
    network->subnet_count = static_cast<unsigned>(subnet_usage.size());
    auto network_size = maze_size;
    network_size.width = maze_size.width * 2 + 1;
    network_size.height = maze_size.height * 2 + 1;
    network->reset(network_size);
    auto cell_to_network_pos = [](const maze::Cell& cell)
    {
        return math::Vec2i{
            static_cast<int>((cell.x * 2) + 1),
            static_cast<int>((cell.y * 2) + 1),
        };
    };

    struct TileData
    {
        TileData(const math::Vec2i& pos, Tile* tile) : pos(pos), tile(tile) {}

        math::Vec2i pos;
        Tile* tile = nullptr;
    };

    std::vector<TileData> available_nodes;
    for (std::size_t y = 0; y < maze.height(); ++y)
    {
        for (std::size_t x = 0; x < maze.width(); ++x)
        {
            const maze::Cell& cell = maze.at(x, y);
            math::Vec2i network_pos = cell_to_network_pos(cell);

            int num_exits = 0;
            for (auto& spec : exits)
            {
                if (cell.exits[spec.dir])
                {
                    ++num_exits;
                    auto* tile = network->get_tile(network_pos + spec.pos_offset);
                    tile->type = TileType::Connector;
                    tile->connector().type = spec.connector_type;
                }
            }

            if (num_exits > 0)
            {
                auto* tile = network->get_tile(network_pos);
                tile->type = TileType::Node;
                tile->node().subnet_id = subnets.at(cell.x, cell.y);
                available_nodes.emplace_back(network_pos, tile);
            }
        }
    }

    network->entrance = cell_to_network_pos(*level_entrance);
    network->exit = cell_to_network_pos(*level_exit);

    network->get_tile(network->entrance)->node().type = NodeType::Entrance;
    network->get_tile(network->exit)->node().type = NodeType::Exit;
    range::erase_if(available_nodes, [](const TileData& data) { return data.tile->node().type != NodeType::Normal; });

    unsigned num_placable_datastores = rng.next(data_stores_in_network.min, data_stores_in_network.max);
    num_placable_datastores = math::min(num_placable_datastores, network->subnet_count);
    std::vector<bool> subnets_with_datastore(network->subnet_count);
    auto available_store_nodes = available_nodes;
    while (!available_store_nodes.empty() && num_placable_datastores > 0)
    {
        auto tile_index = util::choose_index(available_store_nodes, rng);
        TileData data = available_store_nodes[tile_index];
        if (!subnets_with_datastore[data.tile->node().subnet_id])
        {
            subnets_with_datastore[data.tile->node().subnet_id] = true;
            data.tile->node().type = NodeType::DataStore;
            --num_placable_datastores;
        }

        available_store_nodes[tile_index] = available_store_nodes.back();
        available_store_nodes.pop_back();
    }
    range::erase_if(available_nodes, [](const TileData& data) { return data.tile->node().type != NodeType::Normal; });

    unsigned num_placable_enemies = num_patrols;
    network->patrolling_enemies.reserve(num_placable_enemies);
    std::vector<bool> subnets_with_enemies(network->subnet_count);
    auto available_enemy_nodes = available_nodes;
    while (!available_enemy_nodes.empty() && num_placable_enemies > 0)
    {
        auto tile_index = util::choose_index(available_enemy_nodes, rng);
        TileData data = available_enemy_nodes[tile_index];
        auto subnet = data.tile->node().subnet_id;
        if (!subnets_with_enemies[subnet])
        {
            subnets_with_enemies[subnet] = true;
            --num_placable_enemies;

            auto options = find_nodes_in_range(data.pos, patrol_length, network);
            if (!options.empty())
            {
                PatrollingEnemy enemy;
                enemy.point_a = data.pos;
                enemy.point_b = util::choose(options, rng);
                network->patrolling_enemies.push_back(enemy);
            }
        }

        available_enemy_nodes[tile_index] = available_enemy_nodes.back();
        available_enemy_nodes.pop_back();
    }
}
