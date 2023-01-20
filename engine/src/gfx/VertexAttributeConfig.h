#pragma once

#include "RenderConstants.h"
#include "GfxRef.h"

#include <diag/Assert.h>

#include <cstddef>

struct VertexAttributeConfig
{
    BufferRef buffer;
    DataType type = DataType::Float;
    std::size_t count = 0;
    bool normalized = false;
    std::ptrdiff_t step = 0;
    std::ptrdiff_t offset = 0;
    std::size_t attribute_divisor = 0;

    VertexAttributeConfig() = default;
    VertexAttributeConfig(const BufferRef& buffer, DataType type, std::size_t count)
    : buffer(buffer), type(type), count(count), step(get_size(type, count)), offset(0), attribute_divisor(0)
    {}
    VertexAttributeConfig(const BufferRef& buffer, DataType type, std::size_t count, std::ptrdiff_t step, std::ptrdiff_t offset)
    : buffer(buffer), type(type), count(count), step(step), offset(offset), attribute_divisor(0)
    {}

private:
    static std::size_t get_size(DataType type, std::size_t count)
    {
        switch(type)
        {
            default:
                T3D_FAIL("Unknown data type");
            case DataType::Float:
                return sizeof(float) * count;
            case DataType::UnsignedInt:
                return sizeof(unsigned int) * count;
            case DataType::UnsignedShort:
                return sizeof(unsigned short) * count;
            case DataType::UnsignedByte:
                return sizeof(unsigned char) * count;
        }
    }
};

