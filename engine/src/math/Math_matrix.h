#pragma once

#include "Mat22.h"
#include "Mat33.h"
#include "Mat44.h"
#include "Vec3.h"
#include "Quat.h"

namespace math
{
    Mat22f create_rotation_2d(float angle);
    Mat22f create_camera_matrix(float planeX, float planeY, float dirX, float dirY);
    Mat22f invert(const Mat22f& m);

    Mat33f create_rotation_3d(const Vec3f& unit, float angle);

    Mat44f create_ortagonal_projection(float left, float right, float top, float bottom, float near, float far);
    Mat44f create_perspective_projection(float fov, float aspect, float near, float far);

    Mat44f get_translating(const Vec3f& offset);
    Mat44f get_scaling(const Vec3f& scale);
    inline Mat44f get_scaling(float scale) { return get_scaling({scale, scale, scale}); }
    Mat44f get_rotating(const Vec3f& axis, float radians);

    inline Mat44f get_translated(const Mat44f& matrix, const Vec3f& offset) { return get_translating(offset) * matrix; }
    inline Mat44f get_scaled(const Mat44f& matrix, float scale) { return get_scaling(scale) * matrix; }
    inline Mat44f get_scaled(const Mat44f& matrix, const Vec3f& scale) { return get_scaling(scale) * matrix; }
    inline Mat44f get_rotated(const Mat44f& matrix, const Vec3f& axis, float radians) { return get_rotating(axis, radians) * matrix; }

    Mat44f create_look_at(const Vec3f& eye, const Vec3f& target, const Vec3f& up_axis);
    Mat44f create_look_towards(const Vec3f& direction, const Vec3f& up_axis);

    template<typename T>
    Mat33<T> to_mat33(const Quat<T>& q)
    {
        const T qw = q.s;
        const T qx = q.v.x;
        const T qy = q.v.y;
        const T qz = q.v.z;
        return {
            1 - 2*qy*qy - 2*qz*qz,	2*qx*qy - 2*qz*qw,		2*qx*qz + 2*qy*qw,
            2*qx*qy + 2*qz*qw,		1 - 2*qx*qx - 2*qz*qz,	2*qy*qz - 2*qx*qw,
            2*qx*qz - 2*qy*qw,		2*qy*qz + 2*qx*qw,		1 - 2*qx*qx - 2*qy*qy,
        };
    }

    template<typename T>
    Mat44<T> to_mat44(const Mat33<T>& rotation)
    {
        return {
            rotation.buffer[0], rotation.buffer[1], rotation.buffer[2], 0,
            rotation.buffer[3], rotation.buffer[4], rotation.buffer[5], 0,
            rotation.buffer[6], rotation.buffer[7], rotation.buffer[8], 0,
            0, 0, 0, 1,
        };
    }
    
    template<typename T>
    Mat44<T> to_mat44(const Quat<T>& q)
    {
        auto rotation_matrix = to_mat33(q);
        return to_mat44(rotation_matrix);
    }

    template<typename T>
    Vec3<T> to_vec3(const Vec4<T>& vec)
    {
        return {vec.x / vec.w, vec.y / vec.w, vec.z / vec.w};
    }

    template<typename T>
    T get_determinant(const Mat22<T>& matrix)
    {
        return matrix.buffer[0]*matrix.buffer[3] - matrix.buffer[1]*matrix.buffer[2];
    }

    template<typename T>
    T get_determinant(const Mat33<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        const T e = matrix.buffer[4];
        const T f = matrix.buffer[5];
        const T g = matrix.buffer[6];
        const T h = matrix.buffer[7];
        const T i = matrix.buffer[8];
        return
        a * get_determinant(Mat22<T>{e, f, h, i}) -
        b * get_determinant(Mat22<T>{d, f, g, i}) +
        c * get_determinant(Mat22<T>{d, e, g, h});
    }


    template<typename T>
    T get_determinant(const Mat44<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        const T e = matrix.buffer[4];
        const T f = matrix.buffer[5];
        const T g = matrix.buffer[6];
        const T h = matrix.buffer[7];
        const T i = matrix.buffer[8];
        const T j = matrix.buffer[9];
        const T k = matrix.buffer[10];
        const T l = matrix.buffer[11];
        const T m = matrix.buffer[12];
        const T n = matrix.buffer[13];
        const T o = matrix.buffer[14];
        const T p = matrix.buffer[15];
        return
        a * get_determinant(Mat33<T>{f, g, h, j, k, l, n, o, p}) -
        b * get_determinant(Mat33<T>{e, g, h, i, k, l, m, o, p}) +
        c * get_determinant(Mat33<T>{e, f, h, i, j, l, m, n, p}) -
        d * get_determinant(Mat33<T>{e, f, g, i, j, k, m, n, o});
    }

    template<typename T>
    Mat33<T> get_minors(const Mat33<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        const T e = matrix.buffer[4];
        const T f = matrix.buffer[5];
        const T g = matrix.buffer[6];
        const T h = matrix.buffer[7];
        const T i = matrix.buffer[8];
        return {
            get_determinant<T>({e, f, h, i}), get_determinant<T>({d, f, g, i}), get_determinant<T>({d, e, g, h}),
            get_determinant<T>({b, c, h, i}), get_determinant<T>({a, c, g, i}), get_determinant<T>({a, b, g, h}),
            get_determinant<T>({b, c, e, f}), get_determinant<T>({a, c, d, f}), get_determinant<T>({a, b, d, e}),
        };
    }

    template<typename T>
    Mat44<T> get_minors(const Mat44<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        const T e = matrix.buffer[4];
        const T f = matrix.buffer[5];
        const T g = matrix.buffer[6];
        const T h = matrix.buffer[7];
        const T i = matrix.buffer[8];
        const T j = matrix.buffer[9];
        const T k = matrix.buffer[10];
        const T l = matrix.buffer[11];
        const T m = matrix.buffer[12];
        const T n = matrix.buffer[13];
        const T o = matrix.buffer[14];
        const T p = matrix.buffer[15];
        return {
            get_determinant<T>({f, g, h, j, k, l, n, o, p}), get_determinant<T>({e, g, h, i, k, l, m, o, p}), get_determinant<T>({e, f, h, i, j, l, m, n, p}), get_determinant<T>({e, f, g, i, j, k, m, n, o}),
            get_determinant<T>({b, c, d, j, k, l, n, o, p}), get_determinant<T>({a, c, d, i, k, l, m, o, p}), get_determinant<T>({a, b, d, i, j, l, m, n, p}), get_determinant<T>({a, b, c, i, j, k, m, n, o}),
            get_determinant<T>({b, c, d, f, g, h, n, o, p}), get_determinant<T>({a, c, d, e, g, h, m, o, p}), get_determinant<T>({a, b, d, e, f, h, m, n, p}), get_determinant<T>({a, b, c, e, f, g, m, n, o}),
            get_determinant<T>({b, c, d, f, g, h, j, k, l}), get_determinant<T>({a, c, d, e, g, h, i, k, l}), get_determinant<T>({a, b, d, e, f, h, i, j, l}), get_determinant<T>({a, b, c, e, f, g, i, j, k}),
        };
    }

    template<typename T>
    Mat22<T> get_cofactors(const Mat22<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        return {
            a, -b,
            -c, d,
        };
    }

    template<typename T>
    Mat33<T> get_cofactors(const Mat33<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        const T e = matrix.buffer[4];
        const T f = matrix.buffer[5];
        const T g = matrix.buffer[6];
        const T h = matrix.buffer[7];
        const T i = matrix.buffer[8];
        return {
            a, -b, c,
            -d, e, -f,
            g, -h, i,
        };
    }

    template<typename T>
    Mat44<T> get_cofactors(const Mat44<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        const T e = matrix.buffer[4];
        const T f = matrix.buffer[5];
        const T g = matrix.buffer[6];
        const T h = matrix.buffer[7];
        const T i = matrix.buffer[8];
        const T j = matrix.buffer[9];
        const T k = matrix.buffer[10];
        const T l = matrix.buffer[11];
        const T m = matrix.buffer[12];
        const T n = matrix.buffer[13];
        const T o = matrix.buffer[14];
        const T p = matrix.buffer[15];
        return {
            a, -b, c, -d,
            -e, f, -g, h,
            i, -j, k, -l,
            -m, n, -o, p,
        };
    }

    template<typename T>
    Mat22<T> get_transposed(const Mat22<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        return {
            a, c,
            b, d,
        };
    }

    template<typename T>
    Mat33<T> get_transposed(const Mat33<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        const T e = matrix.buffer[4];
        const T f = matrix.buffer[5];
        const T g = matrix.buffer[6];
        const T h = matrix.buffer[7];
        const T i = matrix.buffer[8];
        return {
            a, d, g,
            b, e, h,
            c, f, i,
        };
    }

    template<typename T>
    Mat44<T> get_transposed(const Mat44<T>& matrix)
    {
        const T a = matrix.buffer[0];
        const T b = matrix.buffer[1];
        const T c = matrix.buffer[2];
        const T d = matrix.buffer[3];
        const T e = matrix.buffer[4];
        const T f = matrix.buffer[5];
        const T g = matrix.buffer[6];
        const T h = matrix.buffer[7];
        const T i = matrix.buffer[8];
        const T j = matrix.buffer[9];
        const T k = matrix.buffer[10];
        const T l = matrix.buffer[11];
        const T m = matrix.buffer[12];
        const T n = matrix.buffer[13];
        const T o = matrix.buffer[14];
        const T p = matrix.buffer[15];
        return {
            a, e, i, m,
            b, f, j, n,
            c, g, k, o,
            d, h, l, p,
        };
    }

    template<typename T>
    Mat22<T> get_inverse(const Mat22<T>& matrix)
    {
        return get_cofactors(matrix) * (1 / get_determinant(matrix));
    }

    template<typename T>
    Mat33<T> get_inverse(const Mat33<T>& matrix)
    {
        return get_transposed(get_cofactors(get_minors(matrix))) * (1 / get_determinant(matrix));
    }

    template<typename T>
    Mat44<T> get_inverse(const Mat44<T>& matrix)
    {
        return get_transposed(get_cofactors(get_minors(matrix))) * (1 / get_determinant(matrix));
    }
}
