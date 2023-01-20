#include "SceneStack.h"

#include <diag/Assert.h>

void SceneStack::update_and_render(const SceneArgs& args)
{
    if(scenes.size())
    {
        scenes.back()->update_and_render(args);
    }
}
