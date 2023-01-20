#include "Gizmo.h"
#include "Renderer.h"
#include <model/Model.h>
#include "ShaderSource.h"
#include <math/Math.h>

static const int origin_plane_width = 20;
static const int origin_plane_step = 1;
static const Color origin_plane_color{0.1f, 0.1f, 0.1f};
static const float gizmo_width = 50.0f;
static const float gizmo_offset = 40.0f;
static const std::string gizmo_vsh =
    "#version 330\n"
    "layout(location=0) in vec3 aPos;"
    "layout(location=1) in vec4 aColor;"
    "uniform mat4 mvp;"
    "out vec4 vertex_color;"
    "void main()"
    "{"
    "	gl_Position = mvp * vec4(aPos,1.0);"
    "	vertex_color = aColor;"
    "}";
static const std::string gizmo_fsh =
    "#version 330\n"
    "in vec4 vertex_color;"
    "out vec4 FragColor;"
    "void main()"
    "{"
    "	FragColor = vertex_color;"
    "}";


void Gizmo::upload(Renderer& renderer)
{
    Model gizmo;
    gizmo.add_vertex(math::Vec3f{0.0f, 0.0f, 0.0f}, Color{1.0f, 0.0f, 0.0f});
    gizmo.add_vertex(math::Vec3f{1.0f, 0.0f, 0.0f}, Color{1.0f, 0.0f, 0.0f});
    gizmo.add_vertex(math::Vec3f{0.0f, 0.0f, 0.0f}, Color{0.0f, 1.0f, 0.0f});
    gizmo.add_vertex(math::Vec3f{0.0f, 1.0f, 0.0f}, Color{0.0f, 1.0f, 0.0f});
    gizmo.add_vertex(math::Vec3f{0.0f, 0.0f, 0.0f}, Color{0.0f, 0.0f, 1.0f});
    gizmo.add_vertex(math::Vec3f{0.0f, 0.0f, 1.0f}, Color{0.0f, 0.0f, 1.0f});
    gizmo.indices = {0, 1, 2, 3, 4, 5};
    orientation_mesh = renderer.create_mesh(gizmo, ElementType::Lines);

    Model plane;
    int start_x = -origin_plane_width / 2;
    int start_z = -origin_plane_width / 2;
    int indices = 0;
    for (int x = 0; x <= origin_plane_width; x += origin_plane_step)
    {
        plane.add_vertex(math::Vec3f(static_cast<float>(start_x + x), 0.0f, static_cast<float>(start_z)), origin_plane_color);
        plane.add_vertex(math::Vec3f(static_cast<float>(start_x + x), 0.0f, static_cast<float>(start_z + origin_plane_width)), origin_plane_color);
        plane.indices.push_back(indices++);
        plane.indices.push_back(indices++);
    }
    for (int z = 0; z <= origin_plane_width; z += origin_plane_step)
    {
        plane.add_vertex(math::Vec3f(static_cast<float>(start_x), 0.0f, static_cast<float>(start_z + z)), origin_plane_color);
        plane.add_vertex(math::Vec3f(static_cast<float>(start_x + origin_plane_width), 0.0f, static_cast<float>(start_z + z)), origin_plane_color);
        plane.indices.push_back(indices++);
        plane.indices.push_back(indices++);
    }
    plane_mesh = renderer.create_mesh(plane, ElementType::Lines);

    ShaderSource shader_source("gizmo_shader", gizmo_vsh, gizmo_fsh);
    shader = renderer.create_shader(shader_source);
}

void Gizmo::render_plane(Renderer& renderer, const Camera& scene_camera)
{
    renderer.clear(ClearMethod::Depth);

    auto projection = renderer.get_projection_for_camera(scene_camera);
    auto view = scene_camera.get_view_matrix();
    auto view_projection = projection * view;

    renderer.use(shader);
    renderer.set_uniform(shader, "mvp", view_projection);
    renderer.draw_elements(plane_mesh);
}

void Gizmo::render_orientation(Renderer& renderer, const Camera& scene_camera)
{
    renderer.clear(ClearMethod::Depth);

    auto view_projection = renderer.get_projection_for_camera(scene_camera);
    auto inv_view_projection = math::get_inverse(view_projection);

    auto framebuffer_size = renderer.get_info().framebuffer_size;
    float gizmo_clip_offset_x = (gizmo_offset / framebuffer_size.width) * 2.0f;
    float gizmo_clip_offset_y = (gizmo_offset / framebuffer_size.height) * 2.0f;
    float offset_x = 1.0f - gizmo_clip_offset_x;
    float offset_y = gizmo_clip_offset_y - 1.0f;
    auto offset = math::Vec4f(offset_x, offset_y, 0.0f, 1.0f);
    auto pos = math::to_vec3(inv_view_projection * offset);

    float gizmo_clip_size = (gizmo_width / framebuffer_size.height) * 2.0f;
    auto clip_size_offset = math::Vec4f(0.0f, gizmo_clip_size / 2.0f, 0.0f, 0.0f);
    auto world_top_pos = math::to_vec3(inv_view_projection * (offset + clip_size_offset));
    auto world_size_offset = world_top_pos - pos;
    auto world_scale = math::length(world_size_offset);

    auto model_matrix = math::Mat44f::identity;
    model_matrix = math::get_scaled(model_matrix, world_scale);
    model_matrix = math::get_translated(model_matrix, pos);
    renderer.use(shader);
    const auto scene_rotation_matrix = math::to_mat44(inversed(scene_camera.transform.get_rotation()));
    renderer.set_uniform(shader, "mvp", view_projection * model_matrix * scene_rotation_matrix);
    renderer.draw_elements(orientation_mesh);
}
