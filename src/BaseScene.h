#pragma once

#include "SceneStack.h"
#include "TitleScene.h"
#include "GameScene.h"

class BaseScene : public Scene
{
public:
    virtual void set_scene_stack(SceneStack* stack) override { scene_stack = stack; }
    virtual void update_and_render(const SceneArgs& args) override;

private:
    TitleScene title_scene;
    GameScene game_scene;
    SceneStack* scene_stack = nullptr;
};
