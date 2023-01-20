#pragma once

#include "RenderConstants.h"
#include "GfxRef.h"
#include "VertexAttributeConfig.h"

#include <vector>

struct MeshSource
{
    ElementType element_type = ElementType::Triangles;
    DataType element_format = DataType::UnsignedInt;
    std::size_t element_count = 0;

    BufferRef index_buffer;
    std::vector<VertexAttributeConfig> vertex_attributes;

    std::vector<BufferRef> owned_buffers;
};
