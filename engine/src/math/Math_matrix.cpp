#include "Math_matrix.h"

#include <cmath>

math::Mat22f math::create_rotation_2d(float angle)
{
    math::Mat22f m;
    m.buffer[0] = std::cos(angle);
    m.buffer[1] = -std::sin(angle);
    m.buffer[2] = std::sin(angle);
    m.buffer[3] = std::cos(angle);
    return m;
}

math::Mat22f math::create_camera_matrix(float planeX, float planeY, float dirX, float dirY)
{
    math::Mat22f m;
    m.buffer[0] = planeX;
    m.buffer[1] = dirX;
    m.buffer[2] = planeY;
    m.buffer[3] = dirY;
    return m;
}

math::Mat22f math::invert(const math::Mat22f& m)
{
    math::Mat22f inv;
    inv.buffer[0] = m.buffer[3];
    inv.buffer[3] = m.buffer[0];
    inv.buffer[1] = m.buffer[1] * -1.0f;
    inv.buffer[2] = m.buffer[2] * -1.0f;
    float D = 1.0f / (m.buffer[0] * m.buffer[3] - m.buffer[1] * m.buffer[2]);
    inv *= D;
    return inv;
}

math::Mat33f math::create_rotation_3d(const Vec3f& u, float angle)
{
    const Mat33f uxu {
            u.x * u.x, u.x * u.y, u.x * u.z,
            u.y * u.x, u.y * u.y, u.y * u.z,
            u.z * u.x, u.z * u.y, u.z * u.z,
    };

    const Mat33f ucross {
            0.0f, -u.z,  u.y,
            u.z, 0.0f, -u.x,
            -u.y,  u.x, 0.0f,
    };

    static const Mat33f i {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    };

    return i * std::cos(angle) + ucross * std::sin(angle) + uxu * (1 - std::cos(angle));
}

math::Mat44f math::create_ortagonal_projection(float left, float right, float top, float bottom, float near, float far)
{
    if (left == right || top == bottom || near == far)
    {
        return Mat44f::identity;
    }
    else
    {
        return {
            2.0f / (right - left), 0.0f, 0.0f, -(right + left)/(right - left),
            0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom)/(top - bottom),
            0.0f, 0.0f, -2.0f / (far - near), -(far + near)/(far - near),
            0.0f, 0.0f, 0.0f, 1.0f,
        };
    }
}

math::Mat44f math::create_perspective_projection(float fov, float aspect, float near, float far)
{
    // Source: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
    if ( fov <= 0 || aspect == 0 || near == far )
    {
        return Mat44f::identity;
    }

    const float height = 1.0f / std::tan(0.5f * fov);
    const float width = height / aspect;
    const float one_over_depth = 1.0f / (near - far);

    return {
        width, 0.0f, 0.0f, 0.0f,
        0.0f, height, 0.0f, 0.0f,
        0.0f, 0.0f, (far + near) * one_over_depth, (2.0f * far * near) * one_over_depth,
        0.0f, 0.0f, -1.0f, 0.0f
    };
}

math::Mat44f math::get_translating(const Vec3f& offset)
{
    return {
        1.0f, 0.0f, 0.0f, offset.x,
        0.0f, 1.0f, 0.0f, offset.y,
        0.0f, 0.0f, 1.0f, offset.z,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

math::Mat44f math::get_scaling(const Vec3f& scale)
{
    return {
        scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

math::Mat44f math::get_rotating(const math::Vec3f& axis, float radians)
{
    // Source: https://learnopengl.com/#!Getting-started/Transformations
    const auto norm_axis = normalized(axis);
    const float cos_r = std::cos(radians);
    const float one_min_cos_r = 1.0f - cos_r;
    const float sin_r = std::sin(radians);

    return {
        cos_r + (norm_axis.x * norm_axis.x) * one_min_cos_r, (norm_axis.x * norm_axis.y) * one_min_cos_r - norm_axis.z * sin_r, (norm_axis.x * norm_axis.z) * one_min_cos_r + norm_axis.y * sin_r, 0.0f,
        (norm_axis.y * norm_axis.x) * one_min_cos_r + norm_axis.z * sin_r, cos_r + (norm_axis.y * norm_axis.y) * one_min_cos_r, (norm_axis.y * norm_axis.z) * one_min_cos_r - norm_axis.x * sin_r, 0.0f,
        (norm_axis.z * norm_axis.x) * one_min_cos_r - norm_axis.y * sin_r, (norm_axis.z * norm_axis.y) * one_min_cos_r + norm_axis.x * sin_r, cos_r + (norm_axis.z * norm_axis.z) * one_min_cos_r, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

math::Mat44f math::create_look_at(const Vec3f& eye, const Vec3f& target, const Vec3f& up_axis)
{
    const Mat44f translated = get_translated(Mat44f::identity, -eye);
    const auto direction = target - eye;
    const auto m = create_look_towards(direction, up_axis);
    return m * translated;
}

math::Mat44f math::create_look_towards(const Vec3f& direction, const Vec3f& up_axis)
{
    // Source: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
    // Added normalization of s and u to make sure rotation vector contains unit vectors
    const auto norm_up = normalized(up_axis);

    const auto f = normalized(direction);
    const auto s = normalized(cross(f, norm_up));
    const auto u = normalized(cross(s, f));

    return {
        s.x,  s.y,  s.z, 0.0f,
        u.x,  u.y,  u.z, 0.0f,
        -f.x, -f.y, -f.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
}
