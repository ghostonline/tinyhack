R"(
#version 100

precision mediump float;

uniform sampler2D font;
uniform ivec2 font_size;
uniform ivec2 tile_size;

uniform vec2 sampler_factor;
uniform ivec2 console_size;

uniform sampler2D coords_characters;
uniform sampler2D colors_foreground;
uniform sampler2D colors_background;

varying vec2 internal_coord;

void main()
{
    vec2 console_coord = internal_coord * vec2(console_size);
    vec2 cell_coord = fract(console_coord);

    vec2 sampler_coord = (floor(console_coord) + vec2(0.5, 0.5)) / vec2(console_size) * sampler_factor;
    vec4 color_fg = texture2D(colors_foreground, sampler_coord);
    vec4 color_bg = texture2D(colors_background, sampler_coord);
    vec4 color_char = texture2D(coords_characters, sampler_coord);

    // Prevent bleeding from neighbouring tiles
    cell_coord = clamp(cell_coord, 0.5 / float(tile_size.x), 1.0 - (0.5 / float(tile_size.y)));

    int char_x = int(color_char.g * 255.0 + 0.5);
    int char_y = int(color_char.r * 255.0 + 0.5);
    vec2 char_pos = (vec2(char_x, char_y) + cell_coord) * vec2(tile_size);
    vec2 tile_offset = char_pos / vec2(font_size);
    vec4 char_color = texture2D(font, tile_offset);
    gl_FragColor = mix(color_bg, color_fg, char_color.r);
}
)";
