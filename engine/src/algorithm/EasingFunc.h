#pragma once

namespace easing_func
{
    inline float quad_out(float v) { return -1.0f * v * (v - 2.0f); }
    inline float cube_in(float v) { return v * v * v; }
    inline float cube_out(float v) { --v; return v * v * v + 1; }
}
