#pragma once

#include <ostream>

namespace math
{
template<typename T>
struct Vec2;
template<typename T>
struct Vec3;
template<typename T>
struct Vec4;
}

class StringView;
class Path;

std::ostream& operator <<(std::ostream& stream, const StringView& arg);
std::ostream& operator <<(std::ostream& stream, const Path& arg);
std::ostream& operator <<(std::ostream& stream, const math::Vec4<float>& arg);
std::ostream& operator <<(std::ostream& stream, const math::Vec3<float>& arg);
std::ostream& operator <<(std::ostream& stream, const math::Vec3<int>& arg);
std::ostream& operator <<(std::ostream& stream, const math::Vec2<float>& arg);
std::ostream& operator <<(std::ostream& stream, const math::Vec2<int>& arg);
