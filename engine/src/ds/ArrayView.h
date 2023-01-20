#pragma once

#include <cstddef>

template<typename T>
class ArrayView
{
public:
    typedef std::size_t size_type;

    ArrayView() = default;
    ArrayView(const ArrayView& o) = default;
    ArrayView(ArrayView&& o) = default;

    ArrayView& operator= (const ArrayView&) = default;

    ArrayView(T* ptr, size_type count) : ptr(ptr), count(count) {}
    template<typename OT>
    ArrayView(const ArrayView<OT>& view) : ptr(view.get_ptr()), count(view.get_size()) {}

    T& operator[](size_type index) { return ptr[index]; }
    const T& operator[](size_type index) const { return ptr[index]; }

    const T* get_ptr() const { return ptr; }
    T* get_ptr() { return ptr; }
    size_type get_size() const { return count; }

    operator const T*() const { return ptr; }
    operator T*() { return ptr; }

    operator bool() const { return get_size() > 0; }
    operator ArrayView<const T>() const { return {ptr, count}; }

    ArrayView slice(size_type start, size_type end) const
    {
        return {ptr + start, end - start};
    }

    ArrayView slice_from(size_type start) const
    {
        return {ptr + start, count - start};
    }

    ArrayView slice_until(size_type end) const
    {
        return {ptr, end};
    }

private:
    T* ptr = nullptr;
    size_type count = 0;
};
