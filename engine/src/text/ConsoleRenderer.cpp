#include "ConsoleRenderer.h"

#include "Console.h"
#include "gfx/gl/OpenGLConfig.h"
#include <gfx/Renderer.h>
#include <gfx/MeshSource.h>
#include <gfx/ShaderSource.h>

const char ConsoleVertexShader[] =
#if BACKEND_OPENGL
#include "ConsoleRenderer_vsh_gl.h"
#elif BACKEND_OPENGLES
#include "ConsoleRenderer_vsh_gles.h"
#else
#error Unsupported backend
#endif

const char ConsoleFragmentShader[] =
#if BACKEND_OPENGL
#include "ConsoleRenderer_fsh_gl.h"
#elif BACKEND_OPENGLES
#include "ConsoleRenderer_fsh_gles.h"
#else
#error Unsupported backend
#endif

void ConsoleRenderer::init_resources(Renderer& renderer)
{
    T3D_ASSERT(!initialized);
    initialized = true;

    std::vector<math::Vec2f> vertices =
    {
        {0.f, 0.f},
        {0.f, 1.f},
        {1.f, 1.f},
        {1.f, 0.f}
    };
    std::vector<unsigned char> indices = { 0, 1, 2, 3, 0, 2 };
    BufferRef vertex_buffer = renderer.create_buffer(BufferType::VertexAttributes, vertices);
    BufferRef index_buffer = renderer.create_buffer(BufferType::ElementIndices, indices);

    MeshSource quad_source;
    quad_source.element_count = indices.size();
    quad_source.element_format = DataType::UnsignedByte;
    quad_source.element_type = ElementType::Triangles;
    quad_source.index_buffer = index_buffer;
    quad_source.owned_buffers = { vertex_buffer, index_buffer };
    quad_source.vertex_attributes.emplace_back(vertex_buffer, DataType::Float, 2);
    quad = renderer.create_mesh(quad_source);

    ShaderSource shader_source("console_renderer", ConsoleVertexShader, ConsoleFragmentShader);
    shader = renderer.create_shader(shader_source);

    renderer.use(shader);
    auto mvp = math::get_scaling({2.f, -2.f, 0.f});
    mvp = math::get_translated(mvp, {-1.f, 1.f, 0.f});
    renderer.set_uniform(shader, "mvp", mvp);

    renderer.set_uniform(shader, "colors_foreground", TextureUnit::_1);
    renderer.set_uniform(shader, "colors_background", TextureUnit::_2);
    renderer.set_uniform(shader, "coords_characters", TextureUnit::_3);

    texture_chars = renderer.create_texture();
    texture_foreground = renderer.create_texture();
    texture_background = renderer.create_texture();
}

void ConsoleRenderer::free_resources(Renderer& renderer)
{
    T3D_ASSERT(initialized);
    initialized = false;

    renderer.free_shader(shader);
    renderer.free_texture(texture_chars);
    renderer.free_texture(texture_foreground);
    renderer.free_texture(texture_background);
    renderer.free_mesh(quad);
}

void ConsoleRenderer::set_font(const FontTexture& new_font)
{
    font = new_font;
    font_changed = true;
}

void ConsoleRenderer::render(Renderer& renderer, const Console& console)
{
    if (console_size != console.size)
    {
        console_size = console.size;

        int po2_width = math::nearest_po2(console.size.width);
        int po2_height = math::nearest_po2(console.size.height);

        renderer.set_uniform(shader, "sampler_factor",
            math::Vec2f{console.size.width / static_cast<float>(po2_width), console.size.height / static_cast<float>(po2_height)}
        );

        std::size_t required_texture_size = po2_width * po2_height * 4;
        std::vector<byte> temp_buffer(required_texture_size);
        renderer.upload_texture(texture_chars, {po2_width, po2_height}, Image::Format::RGBA, DataType::UnsignedByte, temp_buffer);
        renderer.upload_texture(texture_foreground, {po2_width, po2_height}, Image::Format::RGBA, DataType::UnsignedByte, temp_buffer);
        renderer.upload_texture(texture_background, {po2_width, po2_height}, Image::Format::RGBA, DataType::UnsignedByte, temp_buffer);
    }

    Recti update_area(0, 0, console.size.width, console.size.height);
    renderer.upload_texture_rect(
        texture_foreground,
        update_area,
        Image::Format::RGBA, DataType::UnsignedByte,
        { reinterpret_cast<const byte*>(console.layout_foreground.data()), console.layout_foreground.size() * sizeof(Console::CharColor) }
    );

    renderer.upload_texture_rect(
        texture_background,
        update_area,
        Image::Format::RGBA, DataType::UnsignedByte,
        { reinterpret_cast<const byte*>(console.layout_background.data()), console.layout_background.size() * sizeof(Console::CharColor) }
    );

    T3D_ASSERT(font.grid_width == (1 << 4)); // Check to make sure we can use bit shifting further down
    std::size_t required_size = console.size.width * console.size.height * 4;
    character_texture_buffer.resize(required_size);
    // Use a raw ptr for quick access in loop
    auto* char_texture_array = character_texture_buffer.data();
    auto* char_array = console.layout_character.data();
    int row_index = 0;
    const std::size_t char_array_size = console.layout_character.size();
    for (std::size_t char_index = 0; char_index < char_array_size; ++char_index)
    {
        auto& code = char_array[char_index];
        // We expect rows of 16 tiles, so we can use bit shifting
        char_texture_array[row_index++] = static_cast<byte>(code >> 4);
        char_texture_array[row_index++] = static_cast<byte>(code & 0xF);
        row_index += 2; // Other two channels are unused (for now)
    }

    renderer.upload_texture_rect(
        texture_chars,
        update_area,
        Image::Format::RGBA, DataType::UnsignedByte,
        { character_texture_buffer.data(), required_size }
    );

    renderer.clear();
    renderer.bind(font.texture, TextureUnit::_0);
    renderer.bind(texture_foreground, TextureUnit::_1);
    renderer.bind(texture_background, TextureUnit::_2);
    renderer.bind(texture_chars, TextureUnit::_3);
    renderer.use(shader);

    if (font_changed)
    {
        renderer.set_uniform(shader, "font", TextureUnit::_0);
        renderer.set_uniform(shader, "tile_size", math::Vec2i{font.char_size.width, font.char_size.height});
        renderer.set_uniform(shader, "font_size", math::Vec2i{font.texture_size.width, font.texture_size.height});
    }

    renderer.set_uniform<math::Vec2i>(shader, "console_size", {console.size.width, console.size.height});

    renderer.set_culling_method(CullingMethod::Back);
    renderer.draw_elements(quad);
}
