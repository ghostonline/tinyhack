#pragma once

#include <math/Vec2.h>

#include <vector>

struct ScanAnimation
{
    struct FlashPoint
    {
        float time;
        math::Vec2i pos;
    };

    std::vector<FlashPoint> points;
    float progress = 0.0f;
};
