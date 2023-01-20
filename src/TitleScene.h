#pragma once

#include "SceneStack.h"
#include <text/Console.h>

class TitleScene : public Scene
{
    enum class State
    {
        Initial,
        Loading,
        Rendering,
    };

public:
    virtual void set_scene_stack(SceneStack* stack) override { scene_stack = stack; }
    virtual void update_and_render(const SceneArgs& args) override;

private:
    void start_game();
    void update_loading(ResourceLoader* loader);
    void update_render(const SceneArgs& args);

    State state = State::Initial;
    SceneStack* scene_stack = nullptr;
    Console title_screen;
};
