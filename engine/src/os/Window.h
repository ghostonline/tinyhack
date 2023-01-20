#pragma once

#include <ds/Size2.h>
#include "KeyboardState.h"
#include "MouseState.h"

#include <memory>
#include <functional>

struct GLFWwindow;

class Window
{
	typedef std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> GLFWwindowPtr;
public:
	typedef std::function<void(int, int)> FramebufferResizeCallback;
    typedef Size2i Size;

	static std::unique_ptr<Window> create_windowed(int width, int height, const char* title);

    GLFWwindow* get_raw() { return glfwWindow.get(); }
    const GLFWwindow* get_raw() const { return glfwWindow.get(); }

	void poll_events();
	bool should_close() const;
	void flip();

	Size get_window_size() const { return window_size; }
    Size get_framebuffer_size() const { return framebuffer_size; }
    bool has_focus() const;

    const MouseState& get_mouse_state() const { return mouse; }
    void set_mouse_cursor_position(double x, double y);
    void set_mouse_cursor_visible(bool visible);

    const KeyboardState& get_keyboard_state() const { return keyboard; }

	void set_framebuffer_resize_callback(const FramebufferResizeCallback& callback) { framebufferResizeCallback = callback; }

private:
	Window(GLFWwindowPtr&& glfwWindow);
	void on_framebuffer_resize(int width, int height);
    void on_window_resize(int width, int height);

	GLFWwindowPtr glfwWindow;
    Size window_size;
    Size framebuffer_size;
    MouseState mouse;
    KeyboardState keyboard;
	FramebufferResizeCallback framebufferResizeCallback;
};
