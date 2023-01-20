#pragma once

#include <ds/TimeSpan.h>

#include "KeyboardState.h"
#include "MouseState.h"

struct UpdateArgs
{
    TimeSpan time_since_app_start;

    float delta_time = 0.0f;

    struct
    {
        int step_count = 0;
        float time_per_iteration = 0.0f;
        float time_remaining = 0.0f;
    } fixed_timestep;

    KeyboardState keyboard;
    MouseState mouse;
};
