#pragma once

#include <ecs/EntityID.h>

struct FocusAnimation
{
    ecs::EntityID entity;
    float progress = 0.0f;
};
