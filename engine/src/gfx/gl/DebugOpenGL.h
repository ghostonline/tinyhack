#pragma once

#if OPENGL_DEBUG_ENABLED

#define CONCAT_IMPL(x, y) x ## y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

#include <diag/Assert.h>

namespace debug
{
    bool list_gl_errors();

    struct ScopedGlErrorCheck
    {
        ScopedGlErrorCheck()
        {
            T3D_ASSERT(!debug::list_gl_errors());
        }

        ~ScopedGlErrorCheck()
        {
            T3D_ASSERT(!debug::list_gl_errors());
        }
    };
}

#define CHECK_GL_ERRORS() T3D_ASSERT(!debug::list_gl_errors())
#define SCOPED_GL_ERROR_CHECK() debug::ScopedGlErrorCheck CONCAT(scoped_gl_error_check, __LINE__)

#else

#define CHECK_GL_ERRORS()
#define SCOPED_GL_ERROR_CHECK()

#endif
