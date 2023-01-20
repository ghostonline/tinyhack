#pragma once

class KeyboardState
{
public:
    enum class Key
    {
        Arrow_Left,
        Arrow_Right,
        Arrow_Up,
        Arrow_Down,

        A,
        D,
        W,
        S,
        Q,
        E,
        R,
        H,

        Space,
        Esc,

        Ctrl_Left,
        Ctrl_Right,
        Super_Left,
        Super_Right,
        Shift_Left,
        Shift_Right,

        ForwardSlash,

        NumKeys,
    };

    bool is_pressed(Key key) const { return pressed[static_cast<int>(key)]; }
    bool is_held(Key key) const { return held[static_cast<int>(key)]; }
    bool is_released(Key key) const { return released[static_cast<int>(key)]; }

private:
    bool pressed[static_cast<int>(Key::NumKeys)] = {};
    bool held[static_cast<int>(Key::NumKeys)] = {};
    bool released[static_cast<int>(Key::NumKeys)] = {};

    friend class Window;
};

