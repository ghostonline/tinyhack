#include "VisibilityMap.h"

VisibilityStatus VisibilityMap::get_status(const math::Vec2i& pos) const
{
    const auto relative_pos = pos - offset;
    if (bitmap.contains(relative_pos.x, relative_pos.y))
    {
        return bitmap.at(relative_pos.x, relative_pos.y);
    }
    else
    {
        return VisibilityStatus::None;
    }
}

void VisibilityMap::set_status(const math::Vec2i& pos, VisibilityStatus value)
{
    const auto relative_pos = pos - offset;
    if (bitmap.contains(relative_pos.x, relative_pos.y))
    {
        bitmap.at(relative_pos.x, relative_pos.y) = value;
    }
}
