#pragma once

#include <ds/Size2.h>
#include <gfx/GfxRef.h>
#include <os/GLFW.h>
#include <string>

struct Mesh
{
    GLenum element_type = GL_TRIANGLES;
    GLenum element_format = GL_UNSIGNED_INT;
    GLsizei count = 0;
#if BACKEND_OPENGL
    GLuint vao = 0;
#else
    struct VertexAttribute
    {
        GLuint buffer = 0;
        GLint size = 0;
        GLenum type = GL_NONE;
        GLboolean normalized = GL_FALSE;
        GLsizei stride = 0;
        const GLvoid* pointer = 0;
    };
    GLuint index_buffer = 0;
    std::vector<VertexAttribute> vertex_attributes;
#endif
    std::vector<BufferRef> owned_buffers;
};

struct Texture
{
    GLuint id;
    Size2i size;
};

struct Shader
{
    struct UniformLocation
    {
        GLuint location = 0;
        std::string name;
    };

    GLuint shader_handle;
    GLuint vertex_program;
    GLuint fragment_program;
    std::vector<UniformLocation> uniforms;
};

struct Buffer
{
    GLuint handle;
    GLenum target;
};
