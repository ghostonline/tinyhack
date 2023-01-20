#include "Window.h"
#include "GLFW.h"

#include <diag/Assert.h>
#include <math/Math_misc.h>

static const int KEYBOARD_MAPPING[] =
{
    GLFW_KEY_LEFT,
    GLFW_KEY_RIGHT,
    GLFW_KEY_UP,
    GLFW_KEY_DOWN,
    GLFW_KEY_A,
    GLFW_KEY_D,
    GLFW_KEY_W,
    GLFW_KEY_S,
    GLFW_KEY_Q,
    GLFW_KEY_E,
    GLFW_KEY_R,
    GLFW_KEY_H,
    GLFW_KEY_SPACE,
    GLFW_KEY_ESCAPE,
    GLFW_KEY_LEFT_CONTROL,
    GLFW_KEY_RIGHT_CONTROL,
    GLFW_KEY_LEFT_SUPER,
    GLFW_KEY_RIGHT_SUPER,
    GLFW_KEY_LEFT_SHIFT,
    GLFW_KEY_RIGHT_SHIFT,
    GLFW_KEY_SLASH,
};

static const int MOUSE_MAPPING[] =
{
    GLFW_MOUSE_BUTTON_LEFT,
    GLFW_MOUSE_BUTTON_RIGHT,
    GLFW_MOUSE_BUTTON_MIDDLE,
};

std::unique_ptr<Window> Window::create_windowed(int width, int height, const char* title)
{
	std::unique_ptr<Window> window;
	{
		GLFWwindowPtr glfwWindow {glfwCreateWindow(width, height, title, nullptr, nullptr), glfwDestroyWindow};
		if (!glfwWindow)
		{
			// Failed to open window
			return nullptr;
		}

		window = std::unique_ptr<Window>{ new Window { std::move(glfwWindow) } };
	}

	glfwMakeContextCurrent(window->glfwWindow.get());
	glfwSwapInterval(1);
	glfwSetInputMode(window->glfwWindow.get(), GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetWindowUserPointer(window->glfwWindow.get(), window.get());

	auto framebufferSizeCallback = [](GLFWwindow* glfwWindow, int width, int height)
	{
		Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        T3D_ASSERT(window != nullptr);
		window->on_framebuffer_resize(width, height);
	};
	glfwSetFramebufferSizeCallback(window->glfwWindow.get(), framebufferSizeCallback);
    auto windowSizeCallback = [](GLFWwindow* glfwWindow, int width, int height)
    {
        Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        T3D_ASSERT(window != nullptr);
        window->on_window_resize(width, height);
    };
    glfwSetWindowSizeCallback(window->glfwWindow.get(), windowSizeCallback);
    glfwSetInputMode(window->glfwWindow.get(), GLFW_STICKY_MOUSE_BUTTONS, 1);

	return window;
}

Window::Window(GLFWwindowPtr&& glfwWindow)
: glfwWindow {std::move(glfwWindow)}
{
    glfwGetWindowSize(this->glfwWindow.get(), &window_size.width, &window_size.height);
    glfwGetFramebufferSize(this->glfwWindow.get(), &framebuffer_size.width, &framebuffer_size.height);
}

void Window::on_framebuffer_resize(int width, int height)
{
    framebuffer_size.width = width;
    framebuffer_size.height = height;
	if (framebufferResizeCallback)
	{
		framebufferResizeCallback(width, height);
	}
}

void Window::on_window_resize(int width, int height)
{
    window_size.width = width;
    window_size.height = height;
}

bool Window::has_focus() const
{
#if __EMSCRIPTEN__
    // TODO: Handle this better using callbacks from js (click to focus, click away to unfocus)
    return true;
#else
    return glfwGetWindowAttrib(glfwWindow.get(), GLFW_FOCUSED) != 0;
#endif
}

void Window::set_mouse_cursor_position(double x, double y)
{
    glfwSetCursorPos(glfwWindow.get(), x, y);
}

void Window::set_mouse_cursor_visible(bool visible)
{
    glfwSetInputMode(glfwWindow.get(), GLFW_CURSOR, visible ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
}

void Window::poll_events()
{
	glfwPollEvents();

    auto old_mouse = mouse;
    glfwGetCursorPos(glfwWindow.get(), &mouse.x, &mouse.y);
    mouse.norm_x = mouse.x / window_size.width;
    mouse.norm_y = mouse.y / window_size.height;
    static const int MAX_BUTTONS = static_cast<int>(MouseState::Button::NumButtons);
    for (int ii = 0; ii < MAX_BUTTONS; ++ii)
    {
        auto button_down = glfwGetMouseButton(glfwWindow.get(), MOUSE_MAPPING[ii]) == GLFW_PRESS;
        mouse.pressed[ii] = !old_mouse.held[ii] && button_down;
        mouse.released[ii] = old_mouse.held[ii] && !button_down;
        mouse.held[ii] = button_down;
    }

    auto old_keyboard = keyboard;
    static const int MAX_KEYS = static_cast<int>(KeyboardState::Key::NumKeys);
    for (int ii = 0; ii < MAX_KEYS; ++ii)
    {
        auto key_down = glfwGetKey(glfwWindow.get(), KEYBOARD_MAPPING[ii]) == GLFW_PRESS;
        keyboard.pressed[ii] = !old_keyboard.held[ii] && key_down;
        keyboard.released[ii] = old_keyboard.held[ii] && !key_down;
        keyboard.held[ii] = key_down;
    }
}

bool Window::should_close() const
{
	return glfwWindowShouldClose(glfwWindow.get()) == 1;
}

void Window::flip()
{
	glfwSwapBuffers(glfwWindow.get());
}
