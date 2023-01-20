#include "Application.h"
#include <cstdlib>

#if WIN32
#include <windows.h>
int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow
)
#else
int main(int argc, char* argv[])
#endif
{
    auto context = app::create_context();
    if (context == nullptr)
    {
        return EXIT_FAILURE;
    }

    while (!app::is_finished(context))
    {
        app::run_step(context);
    }

    app::destroy_context(context);

    return EXIT_SUCCESS;
}
