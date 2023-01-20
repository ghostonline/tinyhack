#pragma once

#if __EMSCRIPTEN__
#define BACKEND_OPENGL 0
#define BACKEND_OPENGLES 1
#else
#define BACKEND_OPENGL 1
#define BACKEND_OPENGLES 0
#endif
