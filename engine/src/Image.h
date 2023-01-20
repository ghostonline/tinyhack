#pragma once

#include "ds/Size2.h"
#include "ds/ByteArrayView.h"

#include <vector>

class Image
{
public:
    enum class Format
    {
        RGBA,
    };

    enum class LoadSetting
    {
        None,
        FlipVertically,
    };

    Image() : size{0, 0}, format{Format::RGBA} {}
    Image(const Size2i& size, Format format);

    Size2i get_size() const { return size; }
    Format get_format() const { return format; }

    ByteArrayView get_pixels() { return { pixels.data(), pixels.size() }; }
    ConstByteArrayView get_pixels() const { return { pixels.data(), pixels.size() }; }

    bool load_from_buffer(const ConstByteArrayView& buffer, LoadSetting setting = LoadSetting::None);

    void blit(const Image& src, int x, int y);

private:
    static std::size_t get_pixel_size(Format format);

    Size2i size;
    Format format;
    std::vector<byte> pixels;

};
