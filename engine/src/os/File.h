#pragma once

#include <ds/ArrayView.h>
#include "Path.h"

#include <cstdio>

class File
{
public:
    enum class Origin
    {
        Begin = SEEK_SET,
        Current = SEEK_CUR,
        End = SEEK_END,
    };

    File(const Path& filename, const char* mode) { fp = std::fopen(filename.to_cstr(), mode); }
    ~File() { if (fp) { std::fclose(fp); } }

    File(const File& o) = delete;
    File* operator=(const File& o) = delete;

    bool is_open() const { return fp != nullptr; }
    bool is_eof() { return feof(fp) != 0; }

    bool seek(Origin origin, long offset = 0L) { return fseek(fp, offset, static_cast<int>(origin)) != 0; }
    long tell() { return ftell(fp); }

    template<typename T>
    std::size_t read(ArrayView<T>& buffer) { return fread(buffer, sizeof(T), buffer.get_size(), fp); }

    template<typename T>
    std::size_t write(ArrayView<T>& buffer) { return fwrite(buffer, sizeof(T), buffer.get_size(), fp); }

    template<typename T>
    std::size_t write(const T& data) { return fwrite(&data, sizeof(T), 1, fp); }

private:
    std::FILE* fp = nullptr;
};
