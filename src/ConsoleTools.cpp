#include "ConsoleTools.h"

#include "Palette.h"
#include <text/Console.h>

std::size_t consoletools::get_prompt_size(const StringView& key, const StringView& prompt)
{
    std::size_t prompt_size = 1 + key.get_size() + 1;
    if (prompt.get_size() > 0)
    {
        prompt_size += 1 + prompt.get_size();
    }
    return prompt_size;
}

void consoletools::render_prompt(Console* console, const math::Vec2i& pos, const StringView& key, const StringView& prompt)
{
    auto& normal_color = palette::get(palette::ID::Prompt_Text);
    auto& key_color =  palette::get(palette::ID::Prompt_Key);
    math::Vec2i cursor = pos;
    console->blit(cursor, "[", normal_color);
    cursor.x += 1;
    console->blit(cursor, key, key_color);
    cursor.x += static_cast<int>(key.get_size());
    console->blit(cursor, "]", normal_color);
    if (prompt.get_size() > 0)
    {
        cursor.x += 2; // Include extra space
        console->blit(cursor, prompt, normal_color);
    }
}

void consoletools::render_prompt_centered(Console* console, const math::Vec2i& center, const StringView& key, const StringView& prompt)
{
    std::size_t prompt_size = get_prompt_size(key, prompt);
    math::Vec2i cursor = center;
    cursor.x = center.x - static_cast<int>(prompt_size) / 2;
    render_prompt(console, cursor, key, prompt);
}
