#pragma once

#include "GfxRef.h"
#include "Image.h"
#include "RenderConstants.h"

#include "ds/Color.h"
#include "ds/Rect.h"
#include "ds/StringView.h"

class Camera;
struct ShaderSource;
struct MeshSource;
struct Color;

using UniformHandle = int;

class Renderer
{
public:
    virtual ~Renderer() {}

    struct Info
    {
        Size2i framebuffer_size;
        Recti viewport;
    };

    virtual void push_state() = 0;
    virtual void pop_state() = 0;

    virtual void set_framebuffer_size(const Size2i& size) = 0;
    virtual const Info& get_info() const = 0;

    virtual void set_viewport(const Recti& rect) = 0;
    virtual void set_depth_testing(bool enabled) = 0;
    virtual void set_culling_method(CullingMethod method) = 0;
    virtual void clear(ClearMethod method = ClearMethod::ColorAndDepth, const Color& clear_color = Color::Black) = 0;

    virtual ShaderRef create_shader(const ShaderSource& source) = 0;
    virtual void free_shader(const ShaderRef& shader) = 0;
    virtual void use(const ShaderRef& shader) = 0;
    virtual UniformHandle get_uniform(const ShaderRef& shader, const StringView& name) = 0;
    template<typename UniformType>
    void set_uniform(const ShaderRef& shader, const StringView& name, const UniformType& value);
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const int& value) = 0;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const TextureUnit& value) = 0;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const float& value) = 0;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Mat44f& value) = 0;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const Color& value) = 0;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec2i& value) = 0;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec2f& value) = 0;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec3f& value) = 0;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec4f& value) = 0;

    TextureRef create_texture(const Image& image);
    virtual TextureRef create_texture(const Size2i& size, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer) = 0;
    virtual TextureRef create_texture() = 0;
    virtual void upload_texture(const TextureRef& ref, const Size2i& size, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer) = 0;
    virtual void upload_texture_rect(const TextureRef& ref, const Recti& part, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer) = 0;
    virtual void free_texture(const TextureRef& texture) = 0;

    void set_filter(const TextureRef& texture, TextureFilter filter) { set_filter(texture, filter, filter); }
    virtual void set_filter(const TextureRef& texture, TextureFilter min_filter, TextureFilter mag_filter) = 0;
    void set_wrap_mode(const TextureRef& texture, TextureWrapMode mode) { set_wrap_mode(texture, mode, mode); }
    virtual void set_wrap_mode(const TextureRef& texture, TextureWrapMode mode_horizontal, TextureWrapMode mode_vertical) = 0;
    virtual void bind(const TextureRef& texture, TextureUnit unit = TextureUnit::_0) = 0;

    virtual MeshRef create_mesh(const MeshSource& source) = 0;
    virtual void free_mesh(const MeshRef& mesh) = 0;
    virtual void bind(const MeshRef& mesh) = 0;
    virtual void unbind(const MeshRef& mesh) = 0;
    virtual void draw_elements(const MeshRef& mesh) = 0;
    virtual void draw_elements(ElementType type, DataType format, std::size_t count, const std::uintptr_t buffer_offset) = 0;
    virtual void draw_elements_instanced(const MeshRef& mesh_ref, int count) = 0;

    virtual BufferRef create_buffer(BufferType type) = 0;
    virtual BufferRef create_buffer(BufferType type, const ConstByteArrayView& data) = 0;
    virtual void upload_buffer(const BufferRef& buffer_ref, const ConstByteArrayView& data) = 0;
    virtual void free_buffer(const BufferRef& buffer_ref) = 0;
    virtual void bind(const BufferRef& buffer_ref) = 0;
};

template<typename UniformType>
inline void Renderer::set_uniform(const ShaderRef& shader, const StringView& name, const UniformType& value)
{
    const auto uniform_id = get_uniform(shader, name);
    set_uniform(shader, uniform_id, value);
}

inline TextureRef Renderer::create_texture(const Image& image)
{
    return create_texture(image.get_size(), image.get_format(), DataType::UnsignedByte, image.get_pixels());
}

Renderer* create_platform_renderer();
