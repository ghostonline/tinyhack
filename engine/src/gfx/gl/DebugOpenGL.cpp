#if OPENGL_DEBUG_ENABLED

#include "DebugOpenGL.h"
#include <os/GLFW.h>
#include <diag/Log.h>

bool debug::list_gl_errors()
{
    GLenum errorCode = GL_NO_ERROR;
    bool errors_found = false;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        const char* error = "";
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            default:                               error = "UNKNOWN"; break;
        }
        Log::error("glGetError() = {0}({1})", error, errorCode);
        errors_found = true;
    }

    return errors_found;
}

#endif
