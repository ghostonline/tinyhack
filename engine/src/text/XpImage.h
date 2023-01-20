#pragma once

#include <ds/ByteArrayView.h>
#include <ds/Size2.h>

class XpImage
{
public:
    struct Cell
    {
        std::uint32_t glyph = 0;

        std::uint8_t fg_color_r = 0;
        std::uint8_t fg_color_g = 0;
        std::uint8_t fg_color_b = 0;

        std::uint8_t bg_color_r = 255;
        std::uint8_t bg_color_g = 0;
        std::uint8_t bg_color_b = 255;

        inline bool is_transparent() const { return bg_color_r == 255 && bg_color_g == 0 && bg_color_b == 255; }
    };

    int get_layer_count() const { return static_cast<int>(layer_offsets.size()); }
    Size2i get_layer_size() const { return layer_size; }
    const Cell& get_cell_in_layer(int layer, int x, int y) const;
    Cell get_cell(int x, int y) const;

    bool load_from_buffer(const ConstByteArrayView& buffer);

private:
    Size2i layer_size;
    std::vector<std::size_t> layer_offsets;
    std::vector<byte> data;
};
