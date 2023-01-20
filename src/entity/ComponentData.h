#pragma once

#include <Palette.h>
#include <level/Network.h>
#include <math/Vec2.h>

#include <array>
#include <vector>

struct Position
{
    math::Vec2i pos;
};

struct Sprite
{
    enum class Layer
    {
        None,

        Disabled,
        Default,
        Player,
        Marker,
        Enemy,
    };

    char glyph = 0;
    palette::ID color = palette::ID::Normal;
    Layer layer = Layer::Default;
};

struct VisibleState
{
    bool alerted = false;
    math::Vec2i alert_target;
    bool disabled = false;
};

struct Player
{
    bool attacked = false;
};

struct DisabledStatus
{
    unsigned num_turns = 0;
};

struct PlayerAttacker {};

struct Walker
{
    std::vector<math::Vec2i> walk_path;
    std::size_t path_index = 0;
};

struct AdminAI
{
    bool reset_state = true;
    std::vector<bool> scanned_ids;
};

struct MonitorAI
{
    bool reset_state = true;
    std::array<math::Vec2i, 2> patrol_points;
    std::size_t patrol_index = 0;
};
