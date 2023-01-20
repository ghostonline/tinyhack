#pragma once

struct MouseState
{
    enum class Button
    {
        Left,
        Right,
        Middle,

        NumButtons,
    };

    double x = 0.0;
    double y = 0.0;
    double norm_x = 0.0;
    double norm_y = 0.0;

    bool is_pressed(Button button) const { return pressed[static_cast<int>(button)]; }
    bool is_held(Button button) const { return held[static_cast<int>(button)]; }
    bool is_released(Button button) const { return released[static_cast<int>(button)]; }

private:
    bool pressed[static_cast<int>(Button::NumButtons)];
    bool held[static_cast<int>(Button::NumButtons)];
    bool released[static_cast<int>(Button::NumButtons)];

    friend class Window;
};
