#pragma once

#include <ecs/ECS.h>
#include <math/Vec2.h>

#include <vector>

struct World;
struct Animator;

struct PositionSystem
{
    World* world = nullptr;

    std::vector<ecs::EntityFacade> get_entities_at(const math::Vec2i& pos);
};

struct DisabledStatusSystem
{
    World* world = nullptr;

    void update();
};

struct PlayerAttackSystem
{
    World* world = nullptr;

    void update();
};

struct WalkerSystem
{
    static void create_path(ecs::EntityFacade& entity, const math::Vec2i& from, const math::Vec2i& to, World* world);
    static void update(ecs::EntityFacade& entity);
};

struct SystemAdminAI
{
    World* world = nullptr;
    Animator* animator = nullptr;

    void create(math::Vec2i pos);
    void reset_state(ecs::EntityFacade& entity);
    void update();
    void alert_nearest();
    void send_to_investigate(ecs::EntityFacade& entity, const math::Vec2i& target);
};

struct SystemMonitorAI
{
    World* world = nullptr;
    Animator* animator = nullptr;

    void create(math::Vec2i point_a, math::Vec2i point_b);
    void reset_state(ecs::EntityFacade& entity);
    void update();
};
