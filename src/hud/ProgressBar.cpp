#include "ProgressBar.h"
#include <Palette.h>
#include <StringTools.h>
#include <math/Math_misc.h>
#include <text/Console.h>

static const int ProgressSegmentSize = 5;

int ProgressBar::get_width() const
{
    int progress_max = max * ProgressSegmentSize - 1; // Reduce by one to allow for equal spaced dividers
    int bar_size = progress_max + 2;
    return bar_size;
}

void ProgressBar::draw(Console& console, int y) const
{
    int progress_max = max * ProgressSegmentSize - 1; // Reduce by one to allow for equal spaced dividers
    int bar_size = progress_max + 2;

    math::Vec2i bar_cursor = {(console.size.width - bar_size) / 2, y};
    console.blit_character(bar_cursor, '[', palette::get(palette::ID::Bar_Brackets));
    bar_cursor.x += 1;
    std::vector<char> bar(progress_max, '_');
    for (int index = ProgressSegmentSize - 1; index < progress_max; index += ProgressSegmentSize)
    {
        bar[index] = '|';
    }
    memset(bar.data(), 'O', math::min<unsigned>(progress * ProgressSegmentSize, progress_max));
    console.blit(bar_cursor, {bar.data(), bar.size()}, palette::get(palette::ID::Bar_Progress));
    bar_cursor.x += static_cast<int>(bar.size());
    console.blit_character(bar_cursor, ']', palette::get(palette::ID::Bar_Brackets));
    if (level > 0)
    {
        auto level_str = stringtools::sprintf("x %u", level);
        bar_cursor.x += 2;
        console.blit(bar_cursor, level_str, palette::get(palette::ID::Bar_Multiplier));
    }
}
