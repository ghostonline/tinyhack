#pragma once

#include <math/Vec2.h>

struct FlashPoint
{
    math::Vec2i pos;
    float duration = 1.0f;
    float progress = 0.0f;
};
