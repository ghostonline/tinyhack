#pragma once

#include "SceneStack.h"

class DeathScene : public Scene
{
public:
    virtual void set_scene_stack(SceneStack* stack) override { scene_stack = stack; }
    virtual void update_and_render(const SceneArgs& args) override;

    void set_results(int new_level, int new_score);

private:
    SceneStack* scene_stack = nullptr;
    int level = 0;
    int score = 0;
};

inline void DeathScene::set_results(int new_level, int new_score)
{
    level = new_level;
    score = new_score;
}
