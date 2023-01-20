#pragma once

#include "InputAction.h"

#include <math/Vec2.h>
#include <ds/StringView.h>

#include <bitset>

class KeyboardState;
struct MouseState;

struct Input
{
    static Input create(const KeyboardState& keyboard, const MouseState& mouse);

    StringView get_key_string(InputAction action) const { return key_strings[static_cast<int>(action)]; }
    bool is_pressed(InputAction action) const { return pressed[static_cast<int>(action)]; }
    bool is_held(InputAction action) const { return held[static_cast<int>(action)]; }
    bool is_released(InputAction action) const { return released[static_cast<int>(action)]; }

private:
    StringView key_strings[InputActionCount] = {};
    bool pressed[InputActionCount] = {};
    bool held[InputActionCount] = {};
    bool released[InputActionCount] = {};
};

