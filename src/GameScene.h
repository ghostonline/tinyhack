#pragma once

#include "DeathScene.h"
#include "HelpScene.h"
#include "Palette.h"
#include "SceneStack.h"
#include "animation/Animator.h"
#include "entity/Systems.h"
#include "game/World.h"
#include "hud/ProgressBar.h"
#include "input/Input.h"
#include <text/Console.h>

class GameScene : public Scene
{
    enum class Phase
    {
        StatusUpdates,
        PlayerActions,
        EnemyActions,
        AlarmIncrease,

        _Count,
    };

public:
    virtual void set_scene_stack(SceneStack* stack) override { scene_stack = stack; }
    virtual void update_and_render(const SceneArgs& args) override;

private:
    void init(Random& rng);
    void init_level();
    void go_to_next_level();
    void update(const UpdateArgs* args);
    void update_status(const UpdateArgs* args);
    void update_player(const UpdateArgs* args);
    void update_enemies(const UpdateArgs* args);
    void update_alarm(const UpdateArgs* args);
    void cache_world_map();
    void render_world(Console* console) const;
    void render_hud(Console* console) const;
    void next_phase();
    bool is_player_dead() const;
    bool can_player_download() const;
    bool can_player_hack() const;
    bool can_player_leave() const;
    void push_bar_explosion(const StringView& message = "", const palette::ID& color = palette::ID::Bold);

    bool initialized = false;
    bool world_map_dirty = false;
    bool next_level = false;
    bool player_turn_taken = false;
    Phase phase = Phase::PlayerActions;
    Input input;
    World world;
    Animator animator;
    PositionSystem position_system;
    DisabledStatusSystem disabled_system;
    PlayerAttackSystem attack_system;
    SystemAdminAI admin_ai_system;
    SystemMonitorAI monitor_ai_system;
    Console world_map;
    ProgressBar progress_bar;
    DeathScene death_scene;
    HelpScene help_scene;
    SceneStack* scene_stack;
};
