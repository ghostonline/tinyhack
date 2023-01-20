#pragma once

#include <Palette.h>

#include <string>

struct ProgressBarExplosion
{
    palette::ID color = palette::ID::Normal;
    std::string message;
    int bar_width = 0;
    int bar_y = 0;
    float progress = 0.0f;
};
