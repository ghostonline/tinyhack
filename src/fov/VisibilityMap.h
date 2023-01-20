#pragma once

#include <ds/Array2.h>
#include <math/Vec2.h>

enum class VisibilityStatus
{
    None,
    Obscured,
    Visible,
};

struct VisibilityMap
{
    VisibilityMap() : bitmap(0, 0) {};
    VisibilityMap(math::Vec2i center, std::size_t range) : offset(center), bitmap(range * 2, range * 2)
    {
        offset.x -= static_cast<int>(range);
        offset.y -= static_cast<int>(range);
    }

    bool is_visible(const math::Vec2i& pos) const { return get_status(pos) == VisibilityStatus::Visible; }
    VisibilityStatus get_status(const math::Vec2i& pos) const;
    void set_status(const math::Vec2i& pos, VisibilityStatus value);

    math::Vec2i offset;
    Array2<VisibilityStatus> bitmap;
};
