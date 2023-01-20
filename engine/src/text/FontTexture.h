#pragma once

#include <gfx/GfxRef.h>
#include <ds/Size2.h>

struct FontTexture
{
    TextureRef texture;
    Size2i texture_size;
    Size2i char_size;
    int grid_width;
};
