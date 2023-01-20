#pragma once


namespace detail
{
    inline constexpr int get_input_action_count()
    {
        return
#define INPUT_ACTION(name) 1 +
#include "InputAction_Data.h"
#undef INPUT_ACTION
        0;
    }
}

enum class InputAction
{
#define INPUT_ACTION(name) name,
#include "InputAction_Data.h"
#undef INPUT_ACTION
};

static const int InputActionCount = detail::get_input_action_count();
