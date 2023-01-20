#pragma once

#if ASSERTS_ENABLED

#if __EMSCRIPTEN__
#include <emscripten.h>
// Slightly convoluted syntax to bypass the 'empty variadic argument' warning
#define BREAK_POINT() (EM_ASM("throw $0;", "'ASSERTION FAILED'"))
#else
#if BREAKPOINTS_ENABLED
#include "diag/Debugger.h"
#else
#define BREAK_POINT() (*static_cast<int*>(nullptr) = 1)
#endif
#endif

namespace detail
{
    void log_assert(const char* expression, const char* filename, int line);
}

#define T3D_ASSERT(expr) \
    do { if (!(expr)) { ::detail::log_assert(#expr, __FILE__, __LINE__); BREAK_POINT(); } } while(false)

#define T3D_IF_ELSE_ASSERT(expr) \
    if (!(expr)) { ::detail::log_assert(#expr, __FILE__, __LINE__); BREAK_POINT(); } else

#define T3D_VERIFY(expr, msg) \
    do { if (!(expr)) { ::detail::log_assert(msg, __FILE__, __LINE__); BREAK_POINT(); } } while(false)

#define T3D_FAIL(msg) \
    do { ::detail::log_assert(msg, __FILE__, __LINE__); BREAK_POINT(); } while(false)

#else

namespace detail
{
    template<class T> void ignore( const T& ) { }
}

#define T3D_ASSERT(expr) ::detail::ignore(expr)
#define T3D_IF_ELSE_ASSERT(expr) if (expr)
#define T3D_VERIFY(expr, msg) do { (expr); } while(false)
#define T3D_FAIL(msg) ::detail::ignore(msg)

#endif
