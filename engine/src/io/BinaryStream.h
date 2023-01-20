#pragma once

#include <diag/Assert.h>
#include <ds/ByteArrayView.h>

#include <cstring>

class BinaryStream
{
public:
    using Size = ByteArrayView::size_type;

    BinaryStream(const ByteArrayView& view)
    : view(view)
    , ptr(0)
    {}

    Size get_ptr() const { return ptr; }
    Size get_remaining() const { return view.get_size() > ptr ? view.get_size() - ptr : 0; }

    template<typename Type>
    bool try_read(Type& value) { return try_read(&value, sizeof(Type)); }
    bool try_read(void* buffer, Size size);

    template<typename Type>
    void read(Type& value) { read(&value, sizeof(Type)); }
    template<typename Type>
    Type read() { Type value; read(&value, sizeof(Type)); return value; }
    void read(void* buffer, Size size);

    template<typename Type>
    void peek(Type& value) const { peek(&value, sizeof(Type)); }
    template<typename Type>
    Type peek() { Type value; peek(&value, sizeof(Type)); return value; }
    void peek(void* buffer, Size size) const;

    void skip(Size bytes) { ptr += bytes; }

private:
    ByteArrayView view;
    Size ptr;
};

inline bool BinaryStream::try_read(void* buffer, Size size)
{
    if (ptr + size <= view.get_size())
    {
        read(buffer, size);
        return true;
    }
    else
    {
        return false;
    }
}

inline void BinaryStream::read(void* buffer, Size size)
{
    peek(buffer, size);
    ptr += size;
}

inline void BinaryStream::peek(void* buffer, Size size) const
{
    T3D_ASSERT(ptr + size <= view.get_size());
    std::memcpy(buffer, view.get_ptr() + ptr, size);
}
