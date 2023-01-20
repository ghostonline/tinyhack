#pragma once

#include <memory>
#include <vector>

class SceneStack;
class ResourceLoader;
class Console;
class Random;
struct UpdateArgs;
struct Input;

struct SceneArgs
{
    ResourceLoader* resource_loader = nullptr;
    Console* console = nullptr;
    Random* randomizer = nullptr;
    const UpdateArgs* update_args = nullptr;
    const Input* input = nullptr;
};

class Scene
{
public:
    virtual ~Scene() {}
    virtual void set_scene_stack(SceneStack* stack) = 0;
    virtual void update_and_render(const SceneArgs& args) = 0;
};

class SceneStack
{
public:
    bool has_scenes() const { return scenes.size() > 0; }
    void push_scene(Scene* scene);
    void pop_scene();
    void update_and_render(const SceneArgs& args);

private:
    std::vector<Scene*> scenes;
};

inline void SceneStack::push_scene(Scene* scene)
{
    scene->set_scene_stack(this);
    scenes.push_back(scene);
}

inline void SceneStack::pop_scene()
{
    scenes.back()->set_scene_stack(nullptr);
    scenes.pop_back();
}
