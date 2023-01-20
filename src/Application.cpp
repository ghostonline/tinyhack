#include "Application.h"

#include "BaseScene.h"
#include "resources/ResourceLoader.h"
#include "UpdateArgs.h"
#include "input/Input.h"

#include <Palette.h>
#include <Random.h>
#include <Util.h>
#include <diag/Log.h>
#include <ds/TimeSpan.h>
#include <gfx/Renderer.h>
#include <os/GLFW.h>
#include <os/Window.h>
#include <text/Console.h>
#include <text/ConsoleRenderer.h>

#include <cstdint>
#include <ctime>
#include <cstdint>

using TickStorageType = std::int64_t;
const TickStorageType FRAME_RATE = 1000 / 60; // ticks per frame
const TickStorageType MAX_DELTA_TICKS = 500; // Never go over 0.5 second for dt;
const int FONT_CHAR_WIDTH = 16;
const int FONT_CHAR_HEIGHT = 16;
const int MIN_SCREEN_WIDTH_IN_CHAR = 50;
const int MIN_SCREEN_HEIGHT_IN_CHAR = 40;
const int MIN_SCREEN_WIDTH = MIN_SCREEN_WIDTH_IN_CHAR * FONT_CHAR_WIDTH;
const int MIN_SCREEN_HEIGHT = MIN_SCREEN_HEIGHT_IN_CHAR * FONT_CHAR_HEIGHT;

struct RunContext
{
    GLFW glfw;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    SceneStack scene_stack;
    std::unique_ptr<BaseScene> scene;
    std::unique_ptr<Random> seed_randomizer;
    bool loading = true;
    bool profiler_visible = false;
    ResourceLoader resource_loader;
    Console console;
    ConsoleRenderer console_renderer;
    Input input;

    struct
    {
        TickStorageType ticks_upon_app_launch = 0;
        TickStorageType ticks_after_last_loop = 0;
        TickStorageType ticks_since_last_iteration = 0;
    } timing;
    UpdateArgs update_args;
};

// Returns time in ms
inline TickStorageType getTicks(const GLFW& glfw)
{
    return static_cast<TickStorageType>(glfw.get_time() * 1000.0f);
}

inline int getSeed()
{
    const auto timestamp = std::time(nullptr);
    return static_cast<int>(timestamp);
}

void resize_viewport(RunContext* context, int width, int height)
{
    context->renderer->set_viewport({0, 0, width, height});

    // We do not want the console to become too small for the game
    int clamped_width = math::max(width, MIN_SCREEN_WIDTH);
    int clamped_height = math::max(height, MIN_SCREEN_HEIGHT);

    // This will scale down the console view when it becomes too small
    context->renderer->set_framebuffer_size({clamped_width, clamped_height});

    // Determine whether the console should be resized as well (to preserve pixel perfect-ness)
    int console_width = clamped_width / FONT_CHAR_WIDTH;
    int console_height = clamped_height / FONT_CHAR_HEIGHT;
    const auto& console_size = context->console.size;
    if (console_width != console_size.width || console_height != console_size.height)
    {
        context->console.resize({console_width, console_height});
    }
}

void set_timing_args(RunContext* context, UpdateArgs& args, TickStorageType delta_ticks)
{
    args.delta_time = delta_ticks / 1000.0f;
    args.time_since_app_start += TimeSpan(delta_ticks);
    args.fixed_timestep.step_count = 0;
    context->timing.ticks_since_last_iteration += delta_ticks;
    if (context->timing.ticks_since_last_iteration >= FRAME_RATE)
    {
        int iterations = static_cast<int>(context->timing.ticks_since_last_iteration / FRAME_RATE);
        context->timing.ticks_since_last_iteration -= iterations * FRAME_RATE;
        args.fixed_timestep.step_count = iterations;
    }
    args.fixed_timestep.time_remaining = context->timing.ticks_since_last_iteration / 1000.0f;
}


void init_console(RunContext* context)
{
    T3D_IF_ELSE_ASSERT(context->resource_loader.get_state(ResourceID::Font) == LoadState::Ready)
    {
        FontTexture font;
        font.texture = context->resource_loader.get_texture(ResourceID::Font);
        context->renderer->set_filter(font.texture, TextureFilter::Nearest);
        font.char_size = {FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT};
        font.texture_size = {256, 256};
        font.grid_width = 16;
        context->console_renderer.set_font(font);
    }
}

bool app::is_finished(RunContext* context)
{
    return context->window == nullptr || context->window->should_close();
}

RunContext* app::create_context()
{
    palette::init();

    std::unique_ptr<RunContext> context = t3d::make_unique<RunContext>();
    context->glfw.set_default_window_hints();
    context->window = Window::create_windowed(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT, "TinyHack");
    if (context->window == nullptr)
    {
        Log::error("Failed to create window");
        return nullptr;
    }

    const bool success = context->glfw.load_extensions();
    if (!success)
    {
        Log::error("Failure to initialize OpenGL");
        return nullptr;
    }

    context->renderer.reset(create_platform_renderer());
    auto fb_size = context->window->get_framebuffer_size();
    resize_viewport(context.get(), fb_size.width, fb_size.height);
    context->window->set_framebuffer_resize_callback(std::bind(&resize_viewport, context.get(), std::placeholders::_1, std::placeholders::_2));
    context->console_renderer.init_resources(*context->renderer);

    const int seed = getSeed();
    context->seed_randomizer = t3d::make_unique<Random>(seed);

    context->timing.ticks_upon_app_launch = getTicks(context->glfw);
    context->timing.ticks_after_last_loop = context->timing.ticks_upon_app_launch;

    context->update_args.fixed_timestep.time_per_iteration = FRAME_RATE / 1000.0f;
    set_timing_args(context.get(), context->update_args, 0);

    context->scene = t3d::make_unique<BaseScene>();
    context->scene_stack.push_scene(context->scene.get());

    // Load required display font
    context->resource_loader.load(ResourceID::Font);

    return context.release();
}

void app::run_step(RunContext* context)
{
    const TickStorageType ticks_after_loop = getTicks(context->glfw);
    const TickStorageType delta_ticks = math::clamp<TickStorageType>(ticks_after_loop - context->timing.ticks_after_last_loop, 0, MAX_DELTA_TICKS);
    context->timing.ticks_after_last_loop = ticks_after_loop;
    set_timing_args(context, context->update_args, delta_ticks);
    context->update_args.keyboard = context->window->get_keyboard_state();
    context->update_args.mouse = context->window->get_mouse_state();

    // Process all events
    context->window->poll_events();

    // Handle resource updates
    context->resource_loader.update(*context->renderer);

    if (context->loading)
    {
        // Render nothing
        context->renderer->clear();

        // Are we done yet?
        if (context->resource_loader.get_state(ResourceID::Font) == LoadState::Ready)
        {
            init_console(context);
            context->loading = false;
        }
    }
    else
    {
        // Update input
        context->input = Input::create(context->update_args.keyboard, context->update_args.mouse);

        // Normal game loop
        SceneArgs args;
        args.console = &context->console;
        args.resource_loader = &context->resource_loader;
        args.update_args = &context->update_args;
        args.input = &context->input;
        args.randomizer = context->seed_randomizer.get();
        context->scene_stack.update_and_render(args);
    }
    if (!context->loading) // Cannot render anything without a font
    {
        context->console_renderer.render(*context->renderer, context->console);
    }

    context->window->flip();
}

void app::destroy_context(RunContext* context)
{
    // Release required display resources
    context->resource_loader.release(ResourceID::Font);
    context->resource_loader.update(*context->renderer);
    context->console_renderer.free_resources(*context->renderer);

    delete context;
}
