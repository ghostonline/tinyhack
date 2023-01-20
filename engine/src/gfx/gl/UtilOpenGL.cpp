#include "UtilOpenGL.h"
#include <diag/Log.h>

util::ScopedTextureBind::ScopedTextureBind(GLint texture_id)
{
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    if (last_texture != texture_id)
    {
        glBindTexture(GL_TEXTURE_2D, texture_id);
        restore = true;
    }
}

util::ScopedTextureBind::~ScopedTextureBind()
{
    if (restore)
    {
        glBindTexture(GL_TEXTURE_2D, last_texture);
    }
}

bool util::try_compile(GLuint program, const StringView& source)
{
    const char* source_ptr = source.get_ptr();
    glShaderSource(program, 1, &source_ptr, 0);
    glCompileShader(program);
    GLint status = GL_FALSE;
    glGetShaderiv(program, GL_COMPILE_STATUS, &status);
    return status == GL_TRUE;
}

void util::log_shader_info(const char* shader_name, GLuint shader_handle)
{
    GLint size = 0;
    glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &size);
    char* info_log = static_cast<char*>(alloca(size + 1));

    glGetShaderInfoLog(shader_handle, size, NULL, info_log);
    info_log[size] = 0;
    Log::print("Shader log for {0}:\n{1}", shader_name, info_log);
}

void util::log_program_info(const char* shader_name, GLuint program_handle)
{
    GLint size = 0;
    glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &size);
    char* info_log = static_cast<char*>(alloca(size + 1));

    glGetProgramInfoLog(program_handle, size, NULL, info_log);
    info_log[size] = 0;
    Log::print("Program log for {0}:\n{1}", shader_name, info_log);
}
