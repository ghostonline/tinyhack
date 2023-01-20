#pragma once

#include "SceneStack.h"

class HelpScene : public Scene
{
public:
    virtual void set_scene_stack(SceneStack* stack) override { scene_stack = stack; }
    virtual void update_and_render(const SceneArgs& args) override;

private:
    SceneStack* scene_stack = nullptr;
    float subnet_color_timer = 0.0f;
};
