#include "Animator.h"

#include <Palette.h>
#include <RangeUtil.h>
#include <algorithm/EasingFunc.h>
#include <entity/ComponentData.h>
#include <game/World.h>
#include <math/Math_misc.h>
#include <text/Console.h>

const float Animator::MaxVisibleStateTime = 1.0f;
const float Animator::VisibleStateVisibleTime = 0.5f;
const float Animator::MaxFinalAlarmTime = 0.5f;
const float Animator::FinalAlarmVisibleTime = 0.25f;

static const float ScanAnimationDuration = 0.25f;
static const float FlashPointDuration = 0.5f;
static const float FocusAnimationDuration = 1.0f;
static const float BarExplosionDuration = 2.0f;

template<typename AnimationType>
void progress_simple_animation(std::vector<AnimationType>& animations, float dt, float duration)
{
    float progress_increment = dt / duration;
    for (AnimationType& a : animations)
    {
        a.progress += progress_increment;
    }
    range::erase_if(animations, [](AnimationType& a) { return a.progress > 1.0f; });
}

void Animator::update(float dt)
{
    alert_timer += dt;
    while(alert_timer >= MaxVisibleStateTime)
    {
        alert_timer -= MaxVisibleStateTime;
    }
    final_alarm_timer += dt;
    while (final_alarm_timer >= MaxFinalAlarmTime)
    {
        final_alarm_timer -= MaxFinalAlarmTime;
    }

    for (FlashPoint& point : flash_points)
    {
        point.progress += dt / point.duration;
    }
    range::erase_if(flash_points, [](FlashPoint& a) { return a.progress > 1.0f; });

    float scan_update_increment = dt / ScanAnimationDuration;
    for (ScanAnimation& animation : scan_animations)
    {
        float start = animation.progress;
        float end = animation.progress += scan_update_increment;
        for (auto& point : animation.points)
        {
            if (start < point.time && point.time <= end)
            {
                FlashPoint flash;
                flash.pos = point.pos;
                flash.duration = FlashPointDuration;
                flash_points.push_back(flash);
            }
        }
    }
    range::erase_if(scan_animations, [](ScanAnimation& a) { return a.progress > 1.0f; });

    progress_simple_animation(focus_animations, dt, FocusAnimationDuration);
    progress_simple_animation(bar_explosions, dt, BarExplosionDuration);
}

void Animator::draw_world(Console& console, const Recti& camera_frustum, const World& world) const
{
    math::Vec2i camera_offset{camera_frustum.x, camera_frustum.y};
    Array2<float> flash_impact(console.size.width, console.size.height, 0.0f);
    bool has_flash_points = false;
    for (const FlashPoint& point : flash_points)
    {
        if (camera_frustum.contains(point.pos.x, point.pos.y) && world.get_visibility(point.pos) != Visibility::Hidden)
        {
            float intensity = 1.0f - point.progress;
            auto console_pos = point.pos - camera_offset;
            float current = flash_impact.at(console_pos.x, console_pos.y);
            flash_impact.at(console_pos.x, console_pos.y) = current + intensity;
            has_flash_points = true;
        }
    }

    if (has_flash_points)
    {
        Color flash_color = palette::get(palette::ID::Animation_Flash);

        for (int x = 0; x < static_cast<int>(flash_impact.width()); ++x)
        {
            for (int y = 0; y < static_cast<int>(flash_impact.height()); ++y)
            {
                float impact = flash_impact.at(x, y);
                if (impact > 0.0f)
                {
                    auto base_color = console.get_foreground_color({x, y});
                    auto color = Color::lerp(base_color, flash_color, math::clamp01(impact));
                    console.set_foreground_color({x, y}, color);
                }
            }
        }
    }

    Color focus_color = palette::get(palette::ID::Animation_Focus);
    for (const FocusAnimation& focus : focus_animations)
    {
        auto* position = world.entities.get_component<Position>(focus.entity);
        T3D_ASSERT(position); // Focus animation for entity without position
        bool visible = std::sin(focus.progress * math::PI * 8) > 0.0f; // Make blinking animation
        if (visible && world.get_visibility(position->pos) != Visibility::Hidden)
        {
            auto safe_blit_char = [position, &console, camera_frustum, camera_offset, &focus_color](char c, const math::Vec2i& offset)
            {
                auto pos = position->pos + offset;
                if (camera_frustum.contains(pos.x, pos.y))
                {
                    console.blit_character(pos - camera_offset, c, focus_color);
                }
            };
            safe_blit_char('/', {-1, -1});
            safe_blit_char('/', { 1,  1});
            safe_blit_char('\\', {-1,  1});
            safe_blit_char('\\', { 1, -1});
        }
    }
}

void Animator::draw_hud(Console& console) const
{
    Color base_bracket_color = palette::get(palette::ID::Bar_Brackets);
    Color background_color = palette::get(palette::ID::Background);
    for (const ProgressBarExplosion& explosion : bar_explosions)
    {
        static const float BracketDuration = 0.25f;
        static const float MessageFadeStart = 0.75f;
        static const int MaxBracketFlyout = 10;
        math::Vec2i pos{
            console.size.width / 2,
            explosion.bar_y
        };
        if (explosion.progress < BracketDuration)
        {
            float bracket_progress = explosion.progress / BracketDuration;
            auto bracket_color = Color::lerp(base_bracket_color, background_color, bracket_progress);
            float x_offset = easing_func::cube_out(bracket_progress) * MaxBracketFlyout + explosion.bar_width * 0.5f;
            math::Vec2i left_bracket = pos;
            left_bracket.x -= math::round_to_int(x_offset);
            if (console.size.contains(left_bracket.x, left_bracket.y))
            {
                console.blit_character(left_bracket, '[', bracket_color);
            }
            math::Vec2i right_bracket = pos;
            right_bracket.x += math::round_to_int(x_offset);
            if (console.size.contains(right_bracket.x, right_bracket.y))
            {
                console.blit_character(right_bracket, ']', bracket_color);
            }
        }

        Color color = palette::get(explosion.color);
        if (explosion.progress >= MessageFadeStart)
        {
            float fade_progress = (explosion.progress - MessageFadeStart) / (1.0f - MessageFadeStart);
            float fade_progress_eased = easing_func::cube_out(fade_progress);
            color = Color::lerp(color, background_color, fade_progress_eased);
        }

        auto message_pos = pos;
        message_pos.x -= static_cast<int>(explosion.message.size()) / 2;
        console.blit(message_pos, explosion.message, color);
    }
}
