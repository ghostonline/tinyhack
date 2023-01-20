#include "Renderer.h"
#include <diag/Log.h>
#include <os/GLFW.h>
#include "gl/DebugOpenGL.h"
#include "gl/UtilOpenGL.h"
#include "ShaderSource.h"
#include "MeshSource.h"
#include <RangeUtil.h>
#include "ds/Pool.h"
#include "gl/PrimitivesOpenGL.h"
#include "os/GLFW.h"

#include <iterator>

using namespace util;

static const std::size_t MaxShaders = 256;
static const std::size_t MaxMeshes = 256;
static const std::size_t MaxBuffers = 256;
static const std::size_t MaxTextures = 256;
static const UniformHandle InvalidUniformHandle = -1;

inline bool is_valid(UniformHandle handle)
{
    return handle != InvalidUniformHandle;
}

class Renderer_GL : public Renderer
{
    struct State
    {
        GLenum active_texture;
        GLint program;
        GLint texture;
        GLint sampler;
        GLint array_buffer;
        GLint element_array_buffer;
        GLint vertex_array;
        GLint polygon_mode[2];
        GLint viewport[4];
        GLint scissor_box[4];
        GLenum blend_src_rgb;
        GLenum blend_dst_rgb;
        GLenum blend_src_alpha;
        GLenum blend_dst_alpha;
        GLenum blend_equation_rgb;
        GLenum blend_equation_alpha;
        GLboolean enable_blend;
        GLboolean enable_cull_face;
        GLboolean enable_depth_test;
        GLboolean enable_scissor_test;
    };

public:
    Renderer_GL();
    ~Renderer_GL();

    virtual void push_state() override;
    virtual void pop_state() override;

    virtual void set_framebuffer_size(const Size2i& size) override { info.framebuffer_size = size; }
    virtual const Info& get_info() const override { return info; }

    virtual void set_viewport(const Recti& rect) override;
    virtual void set_depth_testing(bool enabled) override;
    virtual void set_culling_method(CullingMethod method) override;
    virtual void clear(ClearMethod method = ClearMethod::ColorAndDepth, const Color& clear_color = Color::Black) override;

    virtual ShaderRef create_shader(const ShaderSource& source) override;
    virtual void free_shader(const ShaderRef& shader) override;
    virtual void use(const ShaderRef& shader) override;
    virtual UniformHandle get_uniform(const ShaderRef& shader, const StringView& name) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const int& value) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const TextureUnit& value) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const float& value) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Mat44f& value) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const Color& value) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec2i& value) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec2f& value) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec3f& value) override;
    virtual void set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec4f& value) override;

    virtual TextureRef create_texture(const Size2i& size, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer) override;
    virtual TextureRef create_texture() override;
    virtual void upload_texture(const TextureRef& ref, const Size2i& size, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer) override;
    virtual void upload_texture_rect(const TextureRef& ref, const Recti& part, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer) override;
    virtual void free_texture(const TextureRef& texture) override;
    virtual void set_filter(const TextureRef& texture, TextureFilter min_filter, TextureFilter mag_filter) override;
    virtual void set_wrap_mode(const TextureRef& texture, TextureWrapMode mode_horizontal, TextureWrapMode mode_vertical) override;
    virtual void bind(const TextureRef& texture, TextureUnit unit = TextureUnit::_0) override;

    virtual MeshRef create_mesh(const MeshSource& source) override;
    virtual void free_mesh(const MeshRef& mesh) override;
    virtual void bind(const MeshRef& mesh) override;
    virtual void unbind(const MeshRef& mesh_ref) override;
    virtual void draw_elements(const MeshRef& mesh) override;
    virtual void draw_elements(ElementType type, DataType format, std::size_t count, const std::uintptr_t buffer_offset) override;
    virtual void draw_elements_instanced(const MeshRef& mesh_ref, int count) override;

    virtual BufferRef create_buffer(BufferType type) override;
    virtual BufferRef create_buffer(BufferType type, const ConstByteArrayView& data) override;
    virtual void upload_buffer(const BufferRef& buffer_ref, const ConstByteArrayView& data) override;
    virtual void free_buffer(const BufferRef& buffer_ref) override;
    virtual void bind(const BufferRef& buffer_ref) override;

private:
    template<typename SourceType>
    void add_vertex_attribute(MeshSource& source, const std::vector<SourceType>& source_buffer, DataType data_type, std::size_t data_count);

    Info info;
    Pool<Shader> shaders;
    Pool<Texture> textures;
    Pool<Mesh> meshes;
    Pool<Buffer> buffers;
    std::vector<State> state_stack;
};

Renderer* create_platform_renderer()
{
    return new Renderer_GL();
}

template<typename SourceType>
void Renderer_GL::add_vertex_attribute(MeshSource& source, const std::vector<SourceType>& source_buffer, DataType data_type, std::size_t data_count)
{
    if (!source_buffer.empty())
    {
        auto data = source_buffer.data();
        auto item_size = sizeof(SourceType);
        auto buffer_size = source_buffer.size() * item_size;
        auto buffer = create_buffer(BufferType::VertexAttributes, ConstByteArrayView{ data, buffer_size });
        source.owned_buffers.push_back(buffer);
        source.vertex_attributes.emplace_back(buffer, data_type, data_count, item_size, 0);
    }
}

Renderer_GL::Renderer_GL()
    : shaders(MaxShaders)
    , textures(MaxTextures)
    , meshes(MaxMeshes)
    , buffers(MaxBuffers)
{}

Renderer_GL::~Renderer_GL()
{
    for (const auto& item : shaders) { T3D_ASSERT(item.free); }
    for (const auto& item : textures) { T3D_ASSERT(item.free); }
    for (const auto& item : meshes) { T3D_ASSERT(item.free); }
    for (const auto& item : buffers) { T3D_ASSERT(item.free); }
}

void Renderer_GL::push_state()
{
    state_stack.push_back(State());
    State& state = state_stack.back();
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&state.active_texture);
    glGetIntegerv(GL_CURRENT_PROGRAM, &state.program);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &state.texture);
#if BACKEND_OPENGL
    glGetIntegerv(GL_SAMPLER_BINDING, &state.sampler);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &state.array_buffer);
    glGetIntegerv(GL_POLYGON_MODE, state.polygon_mode);
#endif
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &state.element_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &state.vertex_array);
    glGetIntegerv(GL_VIEWPORT, state.viewport);
    glGetIntegerv(GL_SCISSOR_BOX, state.scissor_box);
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&state.blend_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&state.blend_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&state.blend_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&state.blend_dst_alpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&state.blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&state.blend_equation_alpha);
    state.enable_blend = glIsEnabled(GL_BLEND);
    state.enable_cull_face = glIsEnabled(GL_CULL_FACE);
    state.enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    state.enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
}

void Renderer_GL::pop_state()
{
    T3D_ASSERT(!state_stack.empty());
    State& state = state_stack.back();
    glUseProgram(state.program);
    glBindTexture(GL_TEXTURE_2D, state.texture);
#if BACKEND_OPENGL
    glBindSampler(0, state.sampler);
    glBindVertexArray(state.vertex_array);
    glPolygonMode(GL_FRONT_AND_BACK, state.polygon_mode[0]);
#endif
    glActiveTexture(state.active_texture);
    glBindBuffer(GL_ARRAY_BUFFER, state.array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.element_array_buffer);
    glBlendEquationSeparate(state.blend_equation_rgb, state.blend_equation_alpha);
    glBlendFuncSeparate(state.blend_src_rgb, state.blend_dst_rgb, state.blend_src_alpha, state.blend_dst_alpha);
    if (state.enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (state.enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (state.enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (state.enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(state.viewport[0], state.viewport[1], (GLsizei)state.viewport[2], (GLsizei)state.viewport[3]);
    glScissor(state.scissor_box[0], state.scissor_box[1], (GLsizei)state.scissor_box[2], (GLsizei)state.scissor_box[3]);
    state_stack.pop_back();
}

void Renderer_GL::set_viewport(const Recti& rect)
{
    info.viewport = rect;
    glViewport(rect.x, rect.y, rect.width(), rect.height());
}

void Renderer_GL::set_depth_testing(bool enabled)
{
    if (enabled)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer_GL::set_culling_method(CullingMethod method)
{
    bool enabled = false;
    GLenum gl_method = GL_FRONT;
    switch(method)
    {
    case CullingMethod::None:
        enabled = false;
        break;
    case CullingMethod::Front:
        enabled = true;
        gl_method = GL_FRONT;
        break;
    case CullingMethod::Back:
        enabled = true;
        gl_method = GL_BACK;
        break;
    case CullingMethod::FrontAndBack:
        enabled = true;
        gl_method = GL_FRONT_AND_BACK;
        break;
    }

    if (enabled)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(gl_method);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void Renderer_GL::clear(ClearMethod method, const Color& clear_color)
{
    GLbitfield clear_flags = 0;
    if (method == ClearMethod::Color || method == ClearMethod::ColorAndDepth)
    {
        glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        clear_flags |= GL_COLOR_BUFFER_BIT;
    }

    if (method == ClearMethod::Depth || method == ClearMethod::ColorAndDepth)
    {
        clear_flags |= GL_DEPTH_BUFFER_BIT;
    }

    if (clear_flags)
    {
        glClear(clear_flags);
    }
}

ShaderRef Renderer_GL::create_shader(const ShaderSource& source)
{
    auto ref = shaders.add();
    auto shader = shaders.get(ref);

    SCOPED_GL_ERROR_CHECK();

    shader->shader_handle = glCreateProgram();
    T3D_ASSERT(shader->shader_handle != 0);
    shader->vertex_program = glCreateShader(GL_VERTEX_SHADER);
    shader->fragment_program = glCreateShader(GL_FRAGMENT_SHADER);
    if (!try_compile(shader->vertex_program, {source.vertex_program.c_str(), source.vertex_program.size()}))
    {
        log_shader_info(source.program_name.c_str(), shader->vertex_program);
        T3D_FAIL("Unable to compile vertex shader");
    }

    if (!try_compile(shader->fragment_program, {source.fragment_program.c_str(), source.fragment_program.size()}))
    {
        log_shader_info(source.program_name.c_str(), shader->fragment_program);
        T3D_FAIL("Unable to compile fragment shader");
    }

    glAttachShader(shader->shader_handle, shader->vertex_program);
    glAttachShader(shader->shader_handle, shader->fragment_program);
    glLinkProgram(shader->shader_handle);

    GLint link_success = GL_FALSE;
    glGetProgramiv(shader->shader_handle, GL_LINK_STATUS, &link_success);
    if(!link_success)
    {
        log_program_info(source.program_name.c_str(), shader->shader_handle);
        T3D_FAIL("Unable to link shader program");
    }

    return ref;
}

void Renderer_GL::free_shader(const ShaderRef& ref)
{
    auto shader = shaders.get(ref);
    SCOPED_GL_ERROR_CHECK();

    glDetachShader(shader->shader_handle, shader->vertex_program);
    glDeleteShader(shader->vertex_program);

    glDetachShader(shader->shader_handle, shader->fragment_program);
    glDeleteShader(shader->fragment_program);

    glDeleteProgram(shader->shader_handle);
    shaders.remove(ref);
}

void Renderer_GL::use(const ShaderRef& ref)
{
    auto shader = shaders.get(ref);
    glUseProgram(shader->shader_handle);
}

UniformHandle Renderer_GL::get_uniform(const ShaderRef& ref, const StringView& name)
{
    auto shader = shaders.get(ref);

    int uniform_idx = 0;
    std::string name_str(name.get_ptr(), name.get_size());
    for (auto& uniform : shader->uniforms)
    {
        if (uniform.name == name_str)
        {
            return uniform_idx;
        }
        ++uniform_idx;
    }

    uniform_idx = static_cast<int>(shader->uniforms.size());
    shader->uniforms.emplace_back();
    shader->uniforms.back().location = glGetUniformLocation(shader->shader_handle, name_str.c_str());
    shader->uniforms.back().name = name_str;
    CHECK_GL_ERRORS();
    T3D_ASSERT(shader->uniforms.back().location != static_cast<GLuint>(-1));
    return uniform_idx;
}

inline GLuint get_uniform_location(const Pool<Shader>& shaders, const ShaderRef& ref, UniformHandle handle)
{
    T3D_ASSERT(is_valid(handle));
    auto shader = shaders.get(ref);
    return shader->uniforms[handle].location;
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const int& value)
{
    GLuint location = get_uniform_location(shaders, ref, handle);
    glUniform1i(location, value);
    CHECK_GL_ERRORS();
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const TextureUnit& value)
{
    set_uniform(ref, handle, static_cast<int>(value));
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const float& value)
{
    GLuint location = get_uniform_location(shaders, ref, handle);
    glUniform1f(location, value);
    CHECK_GL_ERRORS();
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Mat44f& value)
{
    GLuint location = get_uniform_location(shaders, ref, handle);
#if BACKEND_OPENGL
    glUniformMatrix4fv(location, 1, GL_TRUE, value.buffer);
#else
    // Need to be done manually :/
    auto transposed = math::get_transposed(value);
    glUniformMatrix4fv(location, 1, GL_FALSE, transposed.buffer);
#endif
    CHECK_GL_ERRORS();
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const Color& value)
{
    GLuint location = get_uniform_location(shaders, ref, handle);
    glUniform4f(location, value.r, value.g, value.b, value.a);
    CHECK_GL_ERRORS();
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec2i& value)
{
    GLuint location = get_uniform_location(shaders, ref, handle);
    glUniform2i(location, value.x, value.y);
    CHECK_GL_ERRORS();
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec2f& value)
{
    GLuint location = get_uniform_location(shaders, ref, handle);
    glUniform2f(location, value.x, value.y);
    CHECK_GL_ERRORS();
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec3f& value)
{
    GLuint location = get_uniform_location(shaders, ref, handle);
    glUniform3f(location, value.x, value.y, value.z);
    CHECK_GL_ERRORS();
}

void Renderer_GL::set_uniform(const ShaderRef& ref, UniformHandle handle, const math::Vec4f& value)
{
    GLuint location = get_uniform_location(shaders, ref, handle);
    glUniform4f(location, value.x, value.y, value.z, value.w);
    CHECK_GL_ERRORS();
}

TextureRef Renderer_GL::create_texture(const Size2i& size, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer)
{
    auto ref = create_texture();
    upload_texture(ref, size, format, data_type, buffer);
    return ref;
}

TextureRef Renderer_GL::create_texture()
{
    auto ref = textures.add();
    auto texture = textures.get(ref);
    glGenTextures(1, &texture->id);
    return ref;
}

void Renderer_GL::upload_texture(const TextureRef& ref, const Size2i& size, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer)
{
    SCOPED_GL_ERROR_CHECK();

    auto texture = textures.get(ref);
    texture->size = size;
    util::ScopedTextureBind scope_bind(texture->id);
    Renderer::set_filter(ref, TextureFilter::Linear);
    const GLenum gl_format = to_gl_type(format);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_format, texture->size.width, texture->size.height, 0, gl_format, to_gl_type(data_type), buffer);
}

void Renderer_GL::upload_texture_rect(const TextureRef& ref, const Recti& part, const Image::Format format, DataType data_type, const ConstByteArrayView& buffer)
{
    SCOPED_GL_ERROR_CHECK();

    auto texture = textures.get(ref);
    T3D_ASSERT(texture->size.contains(part.left, part.top) && texture->size.contains_inclusive(part.right, part.bottom)); // Check for out of bounds
    util::ScopedTextureBind scope_bind(texture->id);
    const GLenum gl_format = to_gl_type(format);
    glTexSubImage2D(GL_TEXTURE_2D, 0, part.left, part.top, part.width(), part.height(), gl_format, to_gl_type(data_type), buffer);
}

void Renderer_GL::free_texture(const TextureRef& ref)
{
    auto texture = textures.get(ref);
    glDeleteTextures(1, &texture->id);
    textures.remove(ref);
}

void Renderer_GL::set_filter(const TextureRef& ref, TextureFilter min_filter, TextureFilter mag_filter)
{
    auto texture = textures.get(ref);
    util::ScopedTextureBind scope_bind(texture->id);

    const GLenum gl_min_filter = to_gl_type(min_filter);
    const GLenum gl_mag_filter = to_gl_type(mag_filter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_mag_filter);
}

void Renderer_GL::set_wrap_mode(const TextureRef& ref, TextureWrapMode mode_horizontal, TextureWrapMode mode_vertical)
{
    auto texture = textures.get(ref);
    util::ScopedTextureBind scope_bind(texture->id);

    const GLenum gl_wrap_horizontal = to_gl_type(mode_horizontal);
    const GLenum gl_wrap_vertical = to_gl_type(mode_vertical);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap_horizontal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap_vertical);
}

void Renderer_GL::bind(const TextureRef& ref, TextureUnit unit)
{
    auto texture = textures.get(ref);
    GLuint gl_unit = GL_TEXTURE0 + static_cast<GLuint>(unit);
    glActiveTexture(gl_unit);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

MeshRef Renderer_GL::create_mesh(const MeshSource& source)
{
    SCOPED_GL_ERROR_CHECK();

    auto ref = meshes.add();
    auto mesh = meshes.get(ref);

    mesh->element_type = to_gl_type(source.element_type);
    mesh->element_format = to_gl_type(source.element_format);
    mesh->owned_buffers = source.owned_buffers;

#if BACKEND_OPENGL
    glGenVertexArrays(1, &mesh->vao);
    glBindVertexArray(mesh->vao);

    bind(source.index_buffer);

    GLuint location = 0;
    for (auto& config : source.vertex_attributes)
    {
        bind(config.buffer);
        glVertexAttribPointer(
            location,
            static_cast<GLint>(config.count),
            util::to_gl_type(config.type),
            util::to_gl_type(config.normalized),
            static_cast<GLsizei>(config.step),
            reinterpret_cast<const void*>(config.offset));
        glEnableVertexAttribArray(location);
        glVertexAttribDivisor(location, static_cast<GLuint>(config.attribute_divisor));
        ++location;
    }
    glBindVertexArray(0);
#else
    mesh->index_buffer = buffers.get(source.index_buffer)->handle;
    for (auto& config : source.vertex_attributes)
    {
        mesh->vertex_attributes.push_back(Mesh::VertexAttribute());
        auto& attribute = mesh->vertex_attributes.back();
        attribute.buffer = buffers.get(config.buffer)->handle;
        attribute.size = static_cast<GLint>(config.count);
        attribute.type = util::to_gl_type(config.type);
        attribute.normalized = util::to_gl_type(config.normalized);
        attribute.stride = static_cast<GLsizei>(config.step);
        attribute.pointer = reinterpret_cast<const void*>(config.offset);
        T3D_ASSERT(config.attribute_divisor == 0); // Attribute divisor is unsupported in OpenGLES
    }
#endif
    mesh->count = static_cast<GLsizei>(source.element_count);

    return ref;
}

void Renderer_GL::free_mesh(const MeshRef& mesh_ref)
{
    SCOPED_GL_ERROR_CHECK();

    auto mesh = meshes.get(mesh_ref);

#if BACKEND_OPENGL
    glDeleteVertexArrays(1, &mesh->vao);
#endif

    for (auto& buffer : mesh->owned_buffers)
    {
        free_buffer(buffer);
    }
    mesh->owned_buffers.clear();
    meshes.remove(mesh_ref);
}

void Renderer_GL::bind(const MeshRef& mesh_ref)
{
    auto mesh = meshes.get(mesh_ref);
#if BACKEND_OPENGL
    glBindVertexArray(mesh->vao);
#else
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
    GLuint location = 0;
    for (const auto& attribute : mesh->vertex_attributes)
    {
        glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer);
        glVertexAttribPointer(
            location,
            attribute.size,
            attribute.type,
            attribute.normalized,
            attribute.stride,
            attribute.pointer);
        glEnableVertexAttribArray(location);
        ++location;
    }
#endif
    CHECK_GL_ERRORS();
}

void Renderer_GL::unbind(const MeshRef& mesh_ref)
{
#if BACKEND_OPENGL
    glBindVertexArray(0);
#else
    auto mesh = meshes.get(mesh_ref);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    for (GLuint location = 0; location < mesh->vertex_attributes.size(); ++location)
    {
        glDisableVertexAttribArray(location);
    }
#endif
    CHECK_GL_ERRORS();
}

void Renderer_GL::draw_elements(const MeshRef& mesh_ref)
{
    bind(mesh_ref);
    auto mesh = meshes.get(mesh_ref);
    glDrawElements(mesh->element_type, mesh->count, mesh->element_format, 0);
    CHECK_GL_ERRORS();
    unbind(mesh_ref);
}

void Renderer_GL::draw_elements_instanced(const MeshRef& mesh_ref, int count)
{
#if BACKEND_OPENGL
    auto mesh = meshes.get(mesh_ref);
    glBindVertexArray(mesh->vao);
    glDrawElementsInstanced(mesh->element_type, mesh->count, mesh->element_format, 0, count);
    glBindVertexArray(0);
    CHECK_GL_ERRORS();
#else
    T3D_FAIL("Unsupported on this platform");
#endif
}

void Renderer_GL::draw_elements(ElementType type, DataType format, std::size_t count, const std::uintptr_t buffer_offset)
{
    glDrawElements(to_gl_type(type), static_cast<GLsizei>(count), to_gl_type(format), reinterpret_cast<const void*>(buffer_offset));
    CHECK_GL_ERRORS();
}

BufferRef Renderer_GL::create_buffer(BufferType type)
{
    auto ref = buffers.add();
    auto buffer = buffers.get(ref);

    buffer->target = to_gl_type(type);
    glGenBuffers(1, &buffer->handle);
    CHECK_GL_ERRORS();

    return ref;
}

BufferRef Renderer_GL::create_buffer(BufferType type, const ConstByteArrayView& data)
{
    auto ref = create_buffer(type);
    upload_buffer(ref, data);
    return ref;
}

void Renderer_GL::upload_buffer(const BufferRef& buffer_ref, const ConstByteArrayView& data)
{
    bind(buffer_ref);
    auto buffer = buffers.get(buffer_ref);
    glBufferData(buffer->target, data.get_size(), data.get_ptr(), GL_STATIC_DRAW);
    glBindBuffer(buffer->target, 0);
    CHECK_GL_ERRORS();
}

void Renderer_GL::free_buffer(const BufferRef& buffer_ref)
{
    auto buffer = buffers.get(buffer_ref);
    glDeleteBuffers(1, &buffer->handle);
    buffers.remove(buffer_ref);
    CHECK_GL_ERRORS();
}

void Renderer_GL::bind(const BufferRef& buffer_ref)
{
    auto buffer = buffers.get(buffer_ref);
    glBindBuffer(buffer->target, buffer->handle);
}
