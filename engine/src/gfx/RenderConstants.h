#pragma once

enum class CullingMethod
{
    None,
    Front,
    Back,
    FrontAndBack,
};

enum class ClearMethod
{
    None,
    Color,
    Depth,
    ColorAndDepth,
};

enum class ElementType
{
    Lines,
    Triangles,
};

enum class DataType
{
    Float,
    UnsignedByte,
    UnsignedShort,
    UnsignedInt,
};

enum class TextureUnit
{
    _0,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
};

enum class TextureFilter
{
    None,
    Nearest,
    Linear,
    NearestMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapNearest,
    LinearMipmapLinear,
};

enum class TextureWrapMode
{
    Repeat,
    MirroredRepeat,
    Clamp,
};

enum class BufferType
{
    VertexAttributes,
    ElementIndices,
};
