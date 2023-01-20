#include "Input.h"

#include <KeyboardState.h>
#include <MouseState.h>

#include <vector>

template<typename KeyType>
struct Binding
{
    Binding(InputAction action, KeyType key) : action(action), key(key) {}
    Binding(InputAction action, KeyType key, const std::string& description) : action(action), key(key), description(description) {}

    InputAction action;
    KeyType key;
    std::string description; // TODO: Allow localized strings
};
using KeyBinding = Binding<KeyboardState::Key>;
using ButtonBinding = Binding<MouseState::Button>;

Input Input::create(const KeyboardState& keyboard, const MouseState& mouse)
{
    static const std::vector<KeyBinding> KeyBindings =
    {
        // TODO: Obtain strings from OS?
        KeyBinding(InputAction::MoveUp, KeyboardState::Key::Arrow_Up, "Up"),
        KeyBinding(InputAction::MoveDown, KeyboardState::Key::Arrow_Down, "Down"),
        KeyBinding(InputAction::MoveLeft, KeyboardState::Key::Arrow_Left, "Left"),
        KeyBinding(InputAction::MoveRight, KeyboardState::Key::Arrow_Right, "Right"),
        KeyBinding(InputAction::MoveUp, KeyboardState::Key::W),
        KeyBinding(InputAction::MoveDown, KeyboardState::Key::S),
        KeyBinding(InputAction::MoveLeft, KeyboardState::Key::A),
        KeyBinding(InputAction::MoveRight, KeyboardState::Key::D),
        KeyBinding(InputAction::Interact, KeyboardState::Key::E, "E"),
        KeyBinding(InputAction::WaitTurn, KeyboardState::Key::Space, "SPACE"),
        KeyBinding(InputAction::PeekNodes, KeyboardState::Key::Space, "SPACE"),
        KeyBinding(InputAction::NextScene, KeyboardState::Key::Space, "SPACE"),
        KeyBinding(InputAction::RestartLevel, KeyboardState::Key::R, "R"),
        KeyBinding(InputAction::ShowHelp, KeyboardState::Key::H, "H"),
    };

    Input input;
    for (auto& binding : KeyBindings)
    {
        std::size_t action_index = static_cast<int>(binding.action);
        input.pressed[action_index] |= keyboard.is_pressed(binding.key);
        input.held[action_index] |= keyboard.is_held(binding.key);
        input.released[action_index] |= keyboard.is_released(binding.key);
        if (binding.description.size())
        {
            input.key_strings[action_index] = binding.description;
        }
    }

    static const std::vector<ButtonBinding> ButtonBindings =
    {
        ButtonBinding(InputAction::MoveToPoint, MouseState::Button::Left, "LMB"),
    };

    for (auto& binding : ButtonBindings)
    {
        std::size_t action_index = static_cast<int>(binding.action);
        input.pressed[action_index] |= mouse.is_pressed(binding.key);
        input.held[action_index] |= mouse.is_held(binding.key);
        input.released[action_index] |= mouse.is_released(binding.key);
    }

    return input;
}
