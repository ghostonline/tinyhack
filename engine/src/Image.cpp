#include "Image.h"

#include <diag/Assert.h>
#include <ds/Rect.h>

#define STBI_ASSERT T3D_ASSERT
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#if WIN32
#pragma warning(push)
#pragma warning(disable: 4505) // unreferenced local function has been removed
#endif
#include <stb_image.h>
#if WIN32
#pragma warning(pop)
#endif

Image::Image(const Size2i& size, Format format)
: size(size)
, format(format)
, pixels(get_pixel_size(format) * size.width * size.height)
{}

std::size_t Image::get_pixel_size(Format format)
{
    switch(format)
    {
        default:
            T3D_FAIL("Unknown format");
        case Format::RGBA:
            return 4;
    }
}

bool Image::load_from_buffer(const ConstByteArrayView& buffer, LoadSetting setting)
{
    int components = 0;
    stbi_set_flip_vertically_on_load(setting == LoadSetting::FlipVertically);
    auto loaded_pixels = stbi_load_from_memory(buffer.get_ptr(), static_cast<int>(buffer.get_size()), &size.width, &size.height, &components, 4);
    if (!loaded_pixels)
    {
        return false;
    }

    format = Format::RGBA;
    pixels.resize(get_pixel_size(format) * size.width * size.height);
    memcpy(pixels.data(), loaded_pixels, pixels.size());
    stbi_image_free(loaded_pixels);
    return true;
}

void Image::blit(const Image& src, int x, int y)
{
    T3D_ASSERT(src.get_format() == format); // Should have the same image format as source image

    Recti dst_rect{x, y, src.get_size().width, src.get_size().height};
    Recti canvas_rect{0, 0, size.width, size.height};
    dst_rect = Recti::get_intersecting_rect(dst_rect, canvas_rect);
    Recti src_rect{0, 0, dst_rect.width(), dst_rect.height()};
    if (x < 0) { src_rect.offset(-x, 0); }
    if (y < 0) { src_rect.offset(0, -y); }

    // Only continue if we actually have something to copy
    if (src_rect.width() > 0 && src_rect.height() > 0)
    {
        int pixel_size = static_cast<int>(get_pixel_size(format));
        int src_pitch = src.get_size().width * pixel_size;
        int src_start_offset = (src_rect.top * src_pitch) + (src_rect.left * pixel_size);
        int dst_pitch = size.width * pixel_size;
        int dst_start_offset = (dst_rect.top * dst_pitch) + (dst_rect.left * pixel_size);
        int copy_size = dst_rect.width() * pixel_size;
        auto src_pixels = src.get_pixels();
        for (int row = 0; row < src_rect.height(); ++row)
        {
            int src_offset = row * src_pitch + src_start_offset;
            int dst_offset = row * dst_pitch + dst_start_offset;
            memcpy(pixels.data() + dst_offset, src_pixels.get_ptr() + src_offset, copy_size);
        }
    }
}
