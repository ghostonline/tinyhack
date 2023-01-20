#pragma once

#include <ds/Rect.h>

class Console;
struct Color;

namespace Box
{

void blit_single_line(Console& console, const Recti& rect, const Color& color);
void blit_double_line(Console& console, const Recti& rect, const Color& color);

};
