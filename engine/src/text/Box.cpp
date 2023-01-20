#include "Box.h"

#include "Console.h"

struct BoxGlyphs
{
    BoxGlyphs(
        Console::CharCodeType horizontal,
        Console::CharCodeType vertical,
        Console::CharCodeType lower_left,
        Console::CharCodeType lower_right,
        Console::CharCodeType upper_left,
        Console::CharCodeType upper_right
    )
    : horizontal(horizontal)
    , vertical(vertical)
    , lower_left(lower_left)
    , lower_right(lower_right)
    , upper_left(upper_left)
    , upper_right(upper_right)
    {}

    Console::CharCodeType horizontal;
    Console::CharCodeType vertical;
    Console::CharCodeType lower_left;
    Console::CharCodeType lower_right;
    Console::CharCodeType upper_left;
    Console::CharCodeType upper_right;
};

static const BoxGlyphs SingleLine(196, 179, 192, 217, 218, 191);
static const BoxGlyphs DoubleLine(205, 186, 200, 188, 201, 187);

void blit(Console& console, const Recti& rect, const Color& color, const BoxGlyphs& glyphs)
{
    console.blit_character({rect.left, rect.top}, glyphs.upper_left, color);
    console.blit_character({rect.right - 1, rect.top}, glyphs.upper_right, color);
    console.blit_character({rect.left, rect.bottom - 1}, glyphs.lower_left, color);
    console.blit_character({rect.right - 1, rect.bottom - 1}, glyphs.lower_right, color);
    for (int x = rect.left + 1; x < rect.right - 1; ++x)
    {
        console.blit_character({x, rect.top}, glyphs.horizontal, color);
        console.blit_character({x, rect.bottom - 1}, glyphs.horizontal, color);
    }
    for (int y = rect.top + 1; y < rect.bottom - 1; ++y)
    {
        console.blit_character({rect.left, y}, glyphs.vertical, color);
        console.blit_character({rect.right - 1, y}, glyphs.vertical, color);
    }
}

void Box::blit_single_line(Console& console, const Recti& rect, const Color& color)
{
    blit(console, rect, color, SingleLine);
}

void Box::blit_double_line(Console& console, const Recti& rect, const Color& color)
{
    blit(console, rect, color, DoubleLine);
}
