#include "Assert.h"

#if ASSERTS_ENABLED

#include "diag/Log.h"

void detail::log_assert(const char* expression, const char* filename, int line)
{
    Log::error("Assertion failed: {0}\nLocation: {1}:{2}", expression, filename, line);
}

#endif
