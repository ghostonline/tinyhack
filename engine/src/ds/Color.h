#pragma once

struct Color
{
    using byte = unsigned char;

    Color() {}
    explicit Color(float v) : r(v), g(v), b(v), a(v) {}
    Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

    Color& operator*=(float rhs);
    Color operator*(float rhs) const;
    Color& operator/=(float rhs);
    Color operator/(float rhs) const;

    static Color from_rgb_uint(unsigned number);
    static Color from_rgb_bytes(byte r, byte g, byte b);
    static Color lerp(const Color& a, const Color& b, float factor);

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

    static const Color White;
    static const Color Grey;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
};

inline Color& Color::operator*=(float rhs)
{
    r *= rhs;
    g *= rhs;
    b *= rhs;
    a *= rhs;
    return *this;
}

inline Color Color::operator*(float rhs) const
{
    Color c = *this;
    c *= rhs;
    return c;
}

inline Color& Color::operator/=(float rhs)
{
    r /= rhs;
    g /= rhs;
    b /= rhs;
    a /= rhs;
    return *this;
}

inline Color Color::operator/(float rhs) const
{
    Color c = *this;
    c /= rhs;
    return c;
}

inline bool operator==(const Color& lhs, const Color& rhs)
{
    return lhs.r == rhs.r
        && lhs.g == rhs.g
        && lhs.b == rhs.b
        && lhs.a == rhs.a;
}

inline bool operator!=(const Color& lhs, const Color& rhs)
{
    return (lhs == rhs) == false;
}

inline Color Color::from_rgb_uint(unsigned number)
{
    const auto get_channel_as_float = [](unsigned number, unsigned offset)
    {
        unsigned char channel_byte = (number >> offset) & 0xFF;
        return static_cast<float>(channel_byte) / static_cast<float>(0xFF);
    };

    Color c;
    c.r = get_channel_as_float(number, 16);
    c.g = get_channel_as_float(number, 8);
    c.b = get_channel_as_float(number, 0);
    c.a = 1;
    return c;
}

inline Color Color::from_rgb_bytes(byte r, byte g, byte b)
{
    const auto get_channel_as_float = [](byte channel_byte)
    {
        return static_cast<float>(channel_byte) / static_cast<float>(0xFF);
    };

    Color c;
    c.r = get_channel_as_float(r);
    c.g = get_channel_as_float(g);
    c.b = get_channel_as_float(b);
    c.a = 1.0f;
    return c;
}

inline Color Color::lerp(const Color& a, const Color& b, float factor)
{
    Color result;
    result.r = a.r + (b.r - a.r) * factor;
    result.g = a.g + (b.g - a.g) * factor;
    result.b = a.b + (b.b - a.b) * factor;
    result.a = a.a + (b.a - a.a) * factor;
    return result;
}
