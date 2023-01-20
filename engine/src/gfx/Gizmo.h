#pragma once
#include "GfxRef.h"
#include "Camera.h"

class Renderer;
class Camera;

class Gizmo
{
public:
    void upload(Renderer& renderer);
    void render_plane(Renderer& renderer, const Camera& scene_camera);
    void render_orientation(Renderer& renderer, const Camera& scene_camera);

private:
    MeshRef orientation_mesh;
    MeshRef plane_mesh;
    ShaderRef shader;
};
