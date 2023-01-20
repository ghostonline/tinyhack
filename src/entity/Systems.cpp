#include "Systems.h"
#include "ComponentData.h"
#include <RangeUtil.h>
#include <RandomUtil.h>
#include <animation/Animator.h>
#include <game/World.h>
#include <level/NetworkTools.h>

std::vector<ecs::EntityFacade> PositionSystem::get_entities_at(const math::Vec2i& pos)
{
    std::vector<ecs::EntityFacade> entities;
    for (auto& entity : world->entities.find_all<Position>())
    {
        if (entity.get_component<Position>()->pos == pos)
        {
            entities.push_back(entity);
        }
    }
    return entities;
}

void DisabledStatusSystem::update()
{
    auto player = world->entities.find_first<Player>();
    auto player_pos = player.get_component<Position>()->pos;

    auto entities = world->entities.find_all<DisabledStatus>();
    for (auto& entity : entities)
    {
        auto* position = entity.get_component<Position>();
        if (position->pos == player_pos)
        {
            continue;
        }

        auto* disabled = entity.get_component<DisabledStatus>();
        --disabled->num_turns;

        auto* visible_state = entity.get_component<VisibleState>();
        if (visible_state)
        {
            visible_state->disabled = disabled->num_turns > 0;
        }

        if (disabled->num_turns == 0)
        {
            entity.remove_component<DisabledStatus>();
        }
    }
}

void PlayerAttackSystem::update()
{
    auto player = world->entities.find_first<Player>();
    auto player_pos = player.get_component<Position>()->pos;

    auto entities = world->entities.find_all<PlayerAttacker, Position>();
    for (auto& entity : entities)
    {
        auto position = entity.get_component<Position>()->pos;
        if (player_pos == position)
        {
            player.get_component<Player>()->attacked = true;
            break;
        }
    }
}

void WalkerSystem::create_path(ecs::EntityFacade& entity, const math::Vec2i& from, const math::Vec2i& to, World* world)
{
    if (from == to) { return; }

    auto new_path = networktools::find_path(world->network, from, to);
    if (!new_path.empty())
    {
        T3D_ASSERT(new_path[0] == from);
        auto* walker = entity.ensure_component<Walker>();
        walker->walk_path = std::move(new_path);
        walker->path_index = 1; // First node is always starting point
    }
}

void WalkerSystem::update(ecs::EntityFacade& entity)
{
    auto* walker = entity.get_component<Walker>();
    auto* position = entity.get_component<Position>();
    T3D_ASSERT(walker && position);
    position->pos = walker->walk_path[walker->path_index];
    ++walker->path_index;
    if (walker->path_index >= walker->walk_path.size())
    {
        // Walk complete, remove component
        entity.remove_component<Walker>();
    }
}

static const ecs::Aspect ActiveAIAspect = ecs::Aspect::all_with<AdminAI>().and_without<DisabledStatus>();

void SystemAdminAI::create(math::Vec2i pos)
{
    T3D_ASSERT(world->network.get_tile_safe(pos)->type == TileType::Node); // Always start out on a network node

    auto admin_enemy = world->entities.create_entity();
    admin_enemy.add_component<AdminAI>();
    admin_enemy.add_component<PlayerAttacker>();
    admin_enemy.add_component<Position>()->pos = pos;
    auto* sprite = admin_enemy.add_component<Sprite>();
    sprite->glyph = 'A';
    sprite->color = palette::ID::Enemy;
    sprite->layer = Sprite::Layer::Enemy;
    admin_enemy.add_component<VisibleState>();
    reset_state(admin_enemy);
}

void SystemAdminAI::reset_state(ecs::EntityFacade& entity)
{
    auto* ai = entity.get_component<AdminAI>();
    auto* visible_state = entity.get_component<VisibleState>();
    ai->reset_state = false;
    ai->scanned_ids.clear();
    ai->scanned_ids.resize(world->network.subnet_count);
    entity.try_remove_component<Walker>();
    visible_state->alerted = false;
}

void SystemAdminAI::update()
{
    auto entities = world->entities.find_all<AdminAI>();
    for (auto& entity : entities)
    {
        auto* ai = entity.get_component<AdminAI>();
        auto* disabled_status = entity.get_component<DisabledStatus>();
        auto* visible_state = entity.get_component<VisibleState>();
        if (disabled_status)
        {
            // Lose all progress and return
            ai->reset_state = true;
            return;
        }

        if (ai->reset_state)
        {
            reset_state(entity);
        }

        auto* position = entity.get_component<Position>();

        auto* current_tile = world->network.get_tile(position->pos);
        auto subnet = current_tile->node().subnet_id;

        auto player = world->entities.find_first<Player>();
        auto player_pos = player.get_component<Position>()->pos;

        if (entity.has_component<Walker>())
        {
            WalkerSystem::update(entity);
        }
        else
        {
            // Scan subnet
            ai->scanned_ids[subnet] = true;
            visible_state->alerted = false;

            // Create scan animation
            {
                ScanAnimation scan_animation;
                networktools::visit_nodes(world->network, position->pos, [subnet, &scan_animation, this](const networktools::VisitData& data)
                {
                    Tile* tile = world->network.get_tile(data.pos);
                    SubnetID tile_subnet = tile->node().subnet_id;
                    if (subnet == tile_subnet)
                    {
                        scan_animation.points.push_back({});
                        scan_animation.points.back().pos = data.pos;
                        scan_animation.points.back().time = static_cast<float>(data.distance_from_start);
                        return networktools::CallbackResult::Continue;
                    }
                    else
                    {
                        return networktools::CallbackResult::StopVisitor;
                    }
                });
                float latest_time = scan_animation.points.back().time;
                for (auto& point : scan_animation.points)
                {
                    point.time /= latest_time;
                }
                animator->scan_animations.push_back(scan_animation);
            }

            // Is player scanned?
            if (world->network.get_tile(player_pos)->node().subnet_id == subnet)
            {
                send_to_investigate(entity, player_pos);
            }
            else
            {
                static const bool unknown_subnet_value = false;
                if (!range::contains(ai->scanned_ids, unknown_subnet_value)) // Need variable due to reference :/
                {
                    ai->scanned_ids.clear();
                    ai->scanned_ids.resize(world->network.subnet_count);
                    ai->scanned_ids[subnet] = true; // We already are in this subnet, so mark this one as done
                }

                SubnetID nearest_unknown_subnet = SubnetIDNull;
                std::vector<math::Vec2i> unknown_nodes;
                auto subnet_finder = [&nearest_unknown_subnet, &unknown_nodes, ai, this](const networktools::VisitData& data)
                {
                    Tile* tile = world->network.get_tile(data.pos);
                    SubnetID subnet = tile->node().subnet_id;
                    if (nearest_unknown_subnet == SubnetIDNull && !ai->scanned_ids[subnet])
                    {
                        nearest_unknown_subnet = subnet;
                    }

                    if (subnet == nearest_unknown_subnet)
                    {
                        unknown_nodes.push_back(data.pos);
                    }

                    return networktools::CallbackResult::Continue;
                };
                networktools::visit_nodes(world->network, position->pos, subnet_finder);

                auto target_node = util::choose(unknown_nodes, world->gameplay_rng);
                WalkerSystem::create_path(entity, position->pos, target_node, world);
            }
        }
    }
}

void SystemAdminAI::alert_nearest()
{
    ecs::EntityFacade nearest;
    int nearest_distance = 0;
    auto entities = world->entities.find_all(ActiveAIAspect);
    auto player_pos = world->entities.find_first<Player>().get_component<Position>()->pos;
    for (auto& entity : entities)
    {
        Position* pos = entity.get_component<Position>();
        int distance = math::distance2(pos->pos, player_pos);
        if (nearest_distance == 0 || nearest_distance > distance)
        {
            nearest = entity;
            nearest_distance = distance;
        }
    }

    if (nearest)
    {
        send_to_investigate(nearest, player_pos);
    }
}

void SystemAdminAI::send_to_investigate(ecs::EntityFacade& entity, const math::Vec2i& target)
{
    VisibleState* visible_state = entity.get_component<VisibleState>();
    visible_state->alerted = true;
    visible_state->alert_target = target;
    FocusAnimation animation;
    animation.entity = entity.entity_id();
    animator->focus_animations.push_back(animation);
    Position* position = entity.get_component<Position>();
    WalkerSystem::create_path(entity, position->pos, target, world);
}

void SystemMonitorAI::create(math::Vec2i point_a, math::Vec2i point_b)
{
    T3D_ASSERT(world->network.get_tile_safe(point_a)->type == TileType::Node); // Always start out on a network node
    T3D_ASSERT(world->network.get_tile_safe(point_b)->type == TileType::Node); // Always end on a network node

    auto entity = world->entities.create_entity();
    auto* ai = entity.add_component<MonitorAI>();
    ai->patrol_points[0] = point_a;
    ai->patrol_points[1] = point_b;
    entity.add_component<PlayerAttacker>();
    entity.add_component<Position>()->pos = ai->patrol_points[0];
    auto* sprite = entity.add_component<Sprite>();
    sprite->glyph = 'a';
    sprite->color = palette::ID::Enemy;
    sprite->layer = Sprite::Layer::Enemy;
    entity.add_component<VisibleState>();
    reset_state(entity);
}

void SystemMonitorAI::reset_state(ecs::EntityFacade& entity)
{
    auto* ai = entity.get_component<MonitorAI>();
    ai->reset_state = false;
    auto* visible_state = entity.get_component<VisibleState>();
    visible_state->alerted = false;
    entity.try_remove_component<Walker>();
}

void SystemMonitorAI::update()
{
    auto entities = world->entities.find_all<MonitorAI>();
    for (auto& entity : entities)
    {
        auto* ai = entity.get_component<MonitorAI>();
        auto* disabled_status = entity.get_component<DisabledStatus>();
        if (disabled_status)
        {
            // Lose all progress and return
            ai->reset_state = true;
            return;
        }

        if (ai->reset_state)
        {
            reset_state(entity);
        }

        auto* position = entity.get_component<Position>();

        if (entity.has_component<Walker>())
        {
            WalkerSystem::update(entity);
        }
        else
        {
            if (position->pos == ai->patrol_points[ai->patrol_index])
            {
                ai->patrol_index = (ai->patrol_index + 1) % ai->patrol_points.size();
            }

            auto target_node = ai->patrol_points[ai->patrol_index];
            WalkerSystem::create_path(entity, position->pos, target_node, world);
        }
    }
}
