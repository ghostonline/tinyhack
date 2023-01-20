#include "BaseScene.h"

void BaseScene::update_and_render(const SceneArgs& args)
{
    // When this scene is started, prepare a brand new game
    scene_stack->push_scene(&game_scene);
    scene_stack->push_scene(&title_scene);
}
