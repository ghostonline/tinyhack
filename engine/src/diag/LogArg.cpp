#include "LogArg.h"

#include <os/Path.h>
#include <math/Vec2.h>
#include <math/Vec3.h>
#include <math/Vec4.h>

std::ostream& operator <<(std::ostream& stream, const StringView& arg)
{
    stream.write(arg.get_ptr(), arg.get_size());
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const Path& arg)
{
    auto view = arg.to_view();
    stream.write(view.get_ptr(), view.get_size());
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const math::Vec4f& arg)
{
    stream << "{" << arg.x << ", " << arg.y << ", " << arg.z << ", " << arg.w << "}";
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const math::Vec3f& arg)
{
    stream << "{" << arg.x << ", " << arg.y << ", " << arg.z << "}";
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const math::Vec3i& arg)
{
    stream << "{" << arg.x << ", " << arg.y << ", " << arg.z << "}";
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const math::Vec2f& arg)
{
    stream << "{" << arg.x << ", " << arg.y << "}";
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const math::Vec2i& arg)
{
    stream << "{" << arg.x << ", " << arg.y << "}";
    return stream;
}
