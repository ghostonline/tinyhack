#pragma once

#include <ds/Array2.h>
#include <ds/Color.h>
#include <ds/Rect.h>
#include <ds/Size2.h>
#include <ds/StringView.h>
#include <math/Vec2.h>

class Console
{
public:
    using CharCodeType = unsigned char;
    using CharColor = unsigned int;
    static_assert(sizeof(CharColor) == 4, "Color should be stored in a 32 bit int");

    using StorageTypeChar = Array2<CharCodeType>;
    using StorageTypeColor = Array2<CharColor>;

    Console() = default;
    Console(const Size2i& size) { resize(size); }

    bool contains(const math::Vec2i& pos) const;

    void resize(const Size2i& new_size);

    void clear();
    void clear_line(int y);
    void clear(const Color& background);
    void clear_line(int y, const Color& background);

    void blit(const Console& source);
    void blit(const math::Vec2i& pos, const Console& source);
    void blit(const math::Vec2i& pos, const Console& source, const Recti& source_rect);

    void blit(const math::Vec2i& pos, CharCodeType c, const Color& foreground, const Color& background);
    void blit_character(const math::Vec2i& pos, CharCodeType c, const Color& foreground);

    void blit(const math::Vec2i& pos, const StringView& text, const Color& foreground);
    void blit(const math::Vec2i& pos, const StringView& text, const Color& foreground, const Color& background);

    void set_foreground_color(const math::Vec2i& pos, const Color& color) { layout_foreground.at(pos.x, pos.y) = convert_color(color); }
    void set_background_color(const math::Vec2i& pos, const Color& color) { layout_background.at(pos.x, pos.y) = convert_color(color); }
    void set_char_code(const math::Vec2i& pos, const CharCodeType& code) { layout_character.at(pos.x, pos.y) = code; }

    Color get_foreground_color(const math::Vec2i& pos) const { return convert_char_color(layout_foreground.at(pos.x, pos.y)); }
    Color get_background_color(const math::Vec2i& pos) const { return convert_char_color(layout_background.at(pos.x, pos.y)); }

    static CharColor convert_color(const Color& color);
    static Color convert_char_color(const CharColor& color);

    Size2i size;
    StorageTypeChar layout_character;
    StorageTypeColor layout_foreground;
    StorageTypeColor layout_background;
};

inline void Console::resize(const Size2i& new_size)
{
    if (size != new_size)
    {
        size = new_size;
        layout_character.resize(size.width, size.height);
        layout_foreground.resize(size.width, size.height);
        layout_background.resize(size.width, size.height);
    }
}

inline bool Console::contains(const math::Vec2i& pos) const
{
    return 0 <= pos.x && pos.x < static_cast<int>(size.width) && 0 <= pos.y && pos.y < static_cast<int>(size.height);
}

inline void Console::blit(const Console& source)
{
    blit(math::Vec2i::Zero, source, {0, 0, source.size.width, source.size.height});
}

inline void Console::blit(const math::Vec2i& pos, const Console& source)
{
    blit(pos, source, {0, 0, source.size.width, source.size.height});
}

inline void Console::blit(const math::Vec2i& pos, CharCodeType c, const Color& foreground, const Color& background)
{
    set_char_code(pos, c);
    set_foreground_color(pos, foreground);
    set_background_color(pos, background);
}

inline void Console::blit_character(const math::Vec2i& pos, CharCodeType c, const Color& foreground)
{
    set_char_code(pos, c);
    set_foreground_color(pos, foreground);
}

inline Console::CharColor Console::convert_color(const Color& color)
{
    return (static_cast<CharColor>(0xFF * color.a) << 24)
        + (static_cast<CharColor>(0xFF * color.b) << 16)
        + (static_cast<CharColor>(0xFF * color.g) <<  8)
        + (static_cast<CharColor>(0xFF * color.r) <<  0);
}

inline Color Console::convert_char_color(const CharColor& char_color)
{
    static const auto get_channel_as_float = [](CharColor number, unsigned offset)
    {
        unsigned char channel_byte = (number >> offset) & 0xFF;
        return static_cast<float>(channel_byte) / static_cast<float>(0xFF);
    };

    Color color;
    color.r = get_channel_as_float(char_color, 0);
    color.g = get_channel_as_float(char_color, 8);
    color.b = get_channel_as_float(char_color, 16);
    color.a = get_channel_as_float(char_color, 24);
    return color;
}
