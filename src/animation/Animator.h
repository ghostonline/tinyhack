#pragma once

#include "ScanAnimation.h"
#include "FlashPoint.h"
#include "FocusAnimation.h"
#include "ProgressBarExplosion.h"
#include <ds/Rect.h>
#include <math/Vec2.h>

#include <vector>

class Console;
struct World;

struct Animator
{
    std::vector<ScanAnimation> scan_animations;
    std::vector<FlashPoint> flash_points;
    std::vector<FocusAnimation> focus_animations;
    std::vector<ProgressBarExplosion> bar_explosions;

    bool is_blocking_animation() const { return false; }
    bool is_alert_visible() const { return alert_timer < VisibleStateVisibleTime; }
    bool is_final_alarm_visible() const { return final_alarm_timer < FinalAlarmVisibleTime; }

    void update(float dt);
    void draw_world(Console& console, const Recti& camera_frustum, const World& world) const;
    void draw_hud(Console& console) const;

private:
    static const float MaxVisibleStateTime;
    static const float VisibleStateVisibleTime;
    static const float MaxFinalAlarmTime;
    static const float FinalAlarmVisibleTime;

    float alert_timer = 0.0f;
    float final_alarm_timer = 0.0f;
};
