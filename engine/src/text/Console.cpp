#include "Console.h"

#include <memory>

void Console::clear()
{
    memset(layout_character.data(), 0, layout_character.size() * sizeof(CharCodeType));
    memset(layout_foreground.data(), 0, layout_foreground.size() * sizeof(CharColor));
    memset(layout_background.data(), 0, layout_background.size() * sizeof(CharColor));
}

void Console::clear_line(int y)
{
    T3D_ASSERT(0 <= y && static_cast<unsigned>(y) < layout_character.height());
    memset(layout_character.data() + layout_character.width() * y, 0, layout_character.width() * sizeof(CharCodeType));
    memset(layout_foreground.data() + layout_foreground.width() * y, 0, layout_foreground.width() * sizeof(CharColor));
    memset(layout_background.data() + layout_background.width() * y, 0, layout_background.width() * sizeof(CharColor));
}

void Console::clear(const Color& background)
{
    clear();
    CharColor bg_value = convert_color(background);
    for (auto& color : layout_background)
    {
        color = bg_value;
    }
}

void Console::clear_line(int y, const Color& background)
{
    clear_line(y);
    CharColor bg_value = convert_color(background);
    auto ptr = layout_background.data() + layout_background.width() * y;
    auto stop = ptr + layout_background.width();
    for (; ptr < stop; ++ptr)
    {
        *ptr = bg_value;
    }
}

void Console::blit(const math::Vec2i& pos, const Console& source, const Recti& desired_source_rect)
{
    Recti console_rect(0,0, size.width, size.height);
    Recti dest_rect = desired_source_rect;
    dest_rect.offset(pos.x, pos.y);
    dest_rect = Recti::get_intersecting_rect(console_rect, dest_rect);
    Recti source_rect = desired_source_rect;
    if (pos.x < 0)
    {
        source_rect.left += -pos.x;
    }
    if (pos.y < 0)
    {
        source_rect.top += -pos.y;
    }

    math::Vec2i src_offset{source_rect.x, source_rect.y};
    math::Vec2i dst_offset{dest_rect.x, dest_rect.y};
    int blit_width = math::min(dest_rect.width(), source_rect.width());
    int blit_height = math::min(dest_rect.height(), source_rect.height());
    for (int y = 0; y < blit_height; ++y)
    {
        auto src_pos = src_offset + math::Vec2i{0, y};
        auto dst_pos = dst_offset + math::Vec2i{0, y};
        auto src_index = src_pos.x + src_pos.y * source.size.width;
        auto dst_index = dst_pos.x + dst_pos.y * size.width;
        memcpy(layout_character.data() + dst_index, source.layout_character.data() + src_index, blit_width * sizeof(CharCodeType));
        memcpy(layout_foreground.data() + dst_index, source.layout_foreground.data() + src_index, blit_width * sizeof(CharColor));
        memcpy(layout_background.data() + dst_index, source.layout_background.data() + src_index, blit_width * sizeof(CharColor));
    }
}

void Console::blit(const math::Vec2i& pos, const StringView& text, const Color& foreground)
{
    auto fg_int = convert_color(foreground);
    int max = static_cast<int>(size.width) - pos.x;
    int text_width = math::min(max, static_cast<int>(text.get_size()));
    for (int code_index = 0; code_index < text_width; ++code_index)
    {
        int x = pos.x + code_index;
        layout_character.at(x, pos.y) = text[code_index];
        layout_foreground.at(x, pos.y) = fg_int;
    }
}

void Console::blit(const math::Vec2i& pos, const StringView& text, const Color& foreground, const Color& background)
{
    auto fg_int = convert_color(foreground);
    auto bg_int = convert_color(background);
    int max = static_cast<int>(size.width) - pos.x;
    int text_width = math::min(max, static_cast<int>(text.get_size()));
    for (int code_index = 0; code_index < text_width; ++code_index)
    {
        int x = pos.x + code_index;
        layout_character.at(x, pos.y) = text[code_index];
        layout_foreground.at(x, pos.y) = fg_int;
        layout_background.at(x, pos.y) = bg_int;
    }
}
