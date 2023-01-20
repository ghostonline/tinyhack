#pragma once

#include "ArrayView.h"

#include <vector>

using byte = unsigned char;
class ByteArrayView : public ArrayView<byte>
{
public:
    using ArrayView::ArrayView;
    ByteArrayView(void* ptr, size_type count)
    : ArrayView(static_cast<byte*>(ptr), count)
    {}
    template<typename ItemType>
    ByteArrayView(std::vector<ItemType>& collection)
    : ArrayView(reinterpret_cast<byte*>(collection.data()), collection.size() * sizeof(ItemType))
    {}
};

class ConstByteArrayView : public ArrayView<const byte>
{
public:
    using ArrayView::ArrayView;
    ConstByteArrayView(const void* ptr, size_type count)
        : ArrayView(static_cast<const byte*>(ptr), count)
    {}
    template<typename ItemType>
    ConstByteArrayView(const std::vector<const ItemType>& collection)
        : ArrayView(reinterpret_cast<const byte*>(collection.data()), collection.size() * sizeof(ItemType))
    {}
    template<typename ItemType>
    ConstByteArrayView(const std::vector<ItemType>& collection)
        : ArrayView(reinterpret_cast<const byte*>(collection.data()), collection.size() * sizeof(ItemType))
    {}
    template<typename ItemType>
    ConstByteArrayView(std::vector<const ItemType>& collection)
        : ArrayView(reinterpret_cast<const byte*>(collection.data()), collection.size() * sizeof(ItemType))
    {}
};
