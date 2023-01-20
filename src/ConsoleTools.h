#pragma once

#include <ds/StringView.h>
#include <math/Vec2.h>

class Console;

namespace consoletools
{
    std::size_t get_prompt_size(const StringView& key, const StringView& prompt = "");
    void render_prompt(Console* console, const math::Vec2i& pos, const StringView& key, const StringView& prompt = "");
    void render_prompt_centered(Console* console, const math::Vec2i& center, const StringView& key, const StringView& prompt = "");
}
