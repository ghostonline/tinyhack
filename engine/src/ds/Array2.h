#pragma once

#include <diag/Assert.h>

#include <vector>

template<typename Type>
class Array2
{
public:
    using item_type = Type;
    using container_type = std::vector<item_type>;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using size_type = typename container_type::size_type;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    Array2() : _width(0), _height(0) {}
    Array2(size_type width, size_type height) : _width(width), _height(height), _data(width * height) {}
    Array2(size_type width, size_type height, const item_type& def) : _width(width), _height(height), _data(width * height, def) {}

    size_type width() const { return _width; }
    size_type height() const { return _height; }
    size_type size() const { return _width * _height; }
    size_type size_in_bytes() const { return _width * _height * sizeof(item_type); }
    bool contains(size_type x, size_type y) const { return x < _width && y < _height; }
    size_type get_index(size_type x, size_type y) const { T3D_ASSERT(contains(x, y)); return x + y * _width; }
    void get_pos(size_type index, size_type* x, size_type* y) const { T3D_ASSERT(index < _data.size()); *x = index % _width; *y = index / _width; }

    void resize(size_type width, size_type height) { _width = width; _height = height; _data.resize(size()); }
    void resize(size_type width, size_type height, const item_type& def) { _width = width; _height = height; _data.resize(size(), def); }

    reference at(size_type x, size_type y) { return _data[get_index(x, y)]; }
    const const_reference at(size_type x, size_type y) const { return _data[get_index(x, y)]; }
    reference at(size_type index) { T3D_ASSERT(index < _data.size()); return _data[index]; }
    const const_reference at(size_type index) const { T3D_ASSERT(index < _data.size()); return _data[index]; }

    item_type* data() { return _data.data(); }
    const item_type* data() const { return _data.data(); }

    iterator begin() { return _data.begin(); }
    iterator end() { return _data.end(); }
    const_iterator begin() const { return _data.begin(); }
    const_iterator end() const { return _data.end(); }

private:
    size_type _width;
    size_type _height;
    container_type _data;
};
