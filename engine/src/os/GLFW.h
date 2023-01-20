#pragma once

#include <diag/Assert.h>
#include <gfx/gl/OpenGLConfig.h>

#if BACKEND_OPENGLES
#include <GLES2/gl2.h>
#elif BACKEND_OPENGL
#include <glad/glad.h>
#else
#error Unsupported OpenGL config
#endif
#include <GLFW/glfw3.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <cstdint>

class GLFW
{
public:
	GLFW();
	~GLFW();
	GLFW(const GLFW&) = delete;
	GLFW(const GLFW&&) = delete;
	GLFW& operator=(const GLFW&) = delete;

	void set_default_window_hints();
	bool load_extensions();
	double get_time() const;
    std::uint64_t get_ticks() const;
    std::uint64_t get_frequency() const;
};

inline GLFW::GLFW()
{
	const int error = glfwInit();
	T3D_ASSERT(error);
}

inline GLFW::~GLFW()
{
	glfwTerminate();
}

inline void GLFW::set_default_window_hints()
{
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
}

inline bool GLFW::load_extensions()
{
#ifndef __EMSCRIPTEN__
	return gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) != 0;
#else
    // Emscripten comes with its own extensions
    return true;
#endif
}

inline double GLFW::get_time() const
{
    return glfwGetTime();
}

inline std::uint64_t GLFW::get_ticks() const
{
#ifndef __EMSCRIPTEN__
    return glfwGetTimerValue();
#else
    return static_cast<std::uint64_t>(emscripten_get_now());
#endif
}

inline std::uint64_t GLFW::get_frequency() const
{
#ifndef __EMSCRIPTEN__
    return glfwGetTimerFrequency();
#else
    return 1000; // Time is in microseconds
#endif
}
