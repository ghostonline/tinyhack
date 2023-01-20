#pragma once

#include <os/GLFW.h>
#include <gfx/VertexAttributeConfig.h>
#include <Image.h>
#include <gfx/Renderer.h>

namespace util
{

struct ScopedTextureBind
{
    ScopedTextureBind(GLint texture_id);
    ~ScopedTextureBind();

    GLint last_texture = 0;
    bool restore = false;
};

inline GLboolean to_gl_type(bool val)
{
    return val ? GL_TRUE : GL_FALSE;
}


inline GLenum to_gl_type(DataType type)
{
    switch(type)
    {
        default:
            T3D_FAIL("Unknown data type");
        case DataType::Float:
            return GL_FLOAT;
        case DataType::UnsignedByte:
            return GL_UNSIGNED_BYTE;
        case DataType::UnsignedShort:
            return GL_UNSIGNED_SHORT;
        case DataType::UnsignedInt:
            return GL_UNSIGNED_INT;
    }
}

inline GLenum to_gl_type(Image::Format format)
{
    switch(format)
    {
        default:
            T3D_FAIL("Unknown image format");
        case Image::Format::RGBA:
            return GL_RGBA;
    }
}

inline GLenum to_gl_type(TextureFilter filter)
{
    switch(filter)
    {
        default:
            T3D_FAIL("Unknown texture filter");
        case TextureFilter::None:
            return GL_NONE;
        case TextureFilter::Nearest:
            return GL_NEAREST;
        case TextureFilter::Linear:
            return GL_LINEAR;
        case TextureFilter::NearestMipmapNearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilter::NearestMipmapLinear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilter::LinearMipmapNearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilter::LinearMipmapLinear:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
}

inline GLenum to_gl_type(TextureWrapMode mode)
{
    switch(mode)
    {
        default:
            T3D_FAIL("Unknown texture wrap mode");
        case TextureWrapMode::Repeat:
            return GL_REPEAT;
        case TextureWrapMode::MirroredRepeat:
            return GL_MIRRORED_REPEAT;
        case TextureWrapMode::Clamp:
            return GL_CLAMP_TO_EDGE;
    }
}

inline GLenum to_gl_type(BufferType type)
{
    switch (type)
    {
    default:
        T3D_FAIL("Unknown buffer type");
    case BufferType::VertexAttributes:
        return GL_ARRAY_BUFFER;
    case BufferType::ElementIndices:
        return GL_ELEMENT_ARRAY_BUFFER;
    }
}

inline GLenum to_gl_type(ElementType elements)
{
    switch(elements)
    {
    default:
        T3D_FAIL("Unknown type");
    case ElementType::Lines:
        return GL_LINES;
    case ElementType::Triangles:
        return GL_TRIANGLES;
    }
}

bool try_compile(GLuint program, const StringView& source);
void log_shader_info(const char* shader_name, GLuint shader_handle);
void log_program_info(const char* shader_name, GLuint program_handle);

}
