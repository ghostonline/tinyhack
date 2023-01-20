#pragma once

#include "FontTexture.h"

#include <vector>

class Console;
class Renderer;

class ConsoleRenderer
{
public:
    void init_resources(Renderer& renderer);
    void free_resources(Renderer& renderer);

    void set_font(const FontTexture& new_font);
    void render(Renderer& renderer, const Console& console);

private:
    bool initialized = false;
    bool font_changed = false;

    std::vector<unsigned char> character_texture_buffer;

    FontTexture font;
    MeshRef quad;
    ShaderRef shader;

    Size2i console_size;
    TextureRef texture_chars;
    TextureRef texture_foreground;
    TextureRef texture_background;
};
