#pragma once

#include <Random.h>
#include <ecs/ECS.h>
#include <level/Network.h>

enum class Visibility
{
    Hidden,
    Detected,
    Visible,
};

struct World
{
private:
    struct SubnetConnection
    {
        SubnetID a;
        SubnetID b;
    };

public:
    int seed = 0;
    int level = 0;
    int max_alarm = 0;
    int current_alarm = 0;
    int current_alarm_level = 0;
    int max_alarm_level = 0;
    int score = 0;
    int download_progress = 0;
    int exit_strength = 0;
    int exit_progress = 0;
    std::vector<bool> known_subnets;
    Array2<Visibility> visibility_map;
    Network network;
    ecs::ECS entities;
    Random gameplay_rng;

    void reset();
    void reset_level();
    Visibility get_visibility(math::Vec2i pos) const { return visibility_map.at(pos.x, pos.y); }
    bool is_subnet_separator(math::Vec2i pos) const;
    bool is_connected_to_subnet(math::Vec2i pos, SubnetID subnet) const;
    bool can_leave() const { return exit_strength <= 0; }

    void update_visibility_map();

private:
    SubnetConnection get_subnet_connections(const math::Vec2i& pos) const;
    bool is_connected_to_visible_node(math::Vec2i pos) const;
};

inline void World::reset()
{
    seed = 0;
    level = 0;
    score = 0;
    reset_level();
}

inline void World::reset_level()
{
    max_alarm = 0;
    current_alarm = 0;
    current_alarm_level = 0;
    max_alarm_level = 0;
    download_progress = 0;
    exit_progress = 0;
    exit_strength = 0;
    known_subnets.clear();
    visibility_map.resize(0,0);
    network = Network();
    entities = ecs::ECS();
}
