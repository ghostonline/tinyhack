#include "Application.h"
#include <emscripten.h>

#include <cstdlib>

int main(int argc, char* argv[])
{
    auto context = app::create_context();
    if (context == nullptr)
    {
        return EXIT_FAILURE;
    }

    auto runner = [](void* context)
    {
        app::run_step(static_cast<RunContext*>(context));
    };
    emscripten_set_main_loop_arg(runner, context, 0, 1);

    return EXIT_SUCCESS;
}
