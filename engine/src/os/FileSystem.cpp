#include "FileSystem.h"
#include "Path.h"
#include "File.h"
#include <diag/Assert.h>
#include <diag/Log.h>

TextBuffer filesystem::load_text_file(const Path& path)
{
    File file{path, "r"};

    TextBuffer buffer;
    if (file.is_open())
    {
        file.seek(File::Origin::End);
        auto size = static_cast<unsigned long>(file.tell());
        file.seek(File::Origin::Begin);
        buffer.resize(size);
        ArrayView<TextBuffer::value_type> buffer_view{buffer.data(), buffer.size()};
        auto read = file.read(buffer_view);
        T3D_ASSERT(size >= read);
        buffer.resize(read);
    }
    else
    {
        Log::error("Unable to open file: {0}", path);
    }

    return buffer;
}

BinaryBuffer filesystem::load_binary_file(const Path& path)
{
    File file{path, "rb"};

    BinaryBuffer buffer;
    if (file.is_open())
    {
        file.seek(File::Origin::End);
        auto size = static_cast<unsigned long>(file.tell());
        file.seek(File::Origin::Begin);
        buffer.resize(size);
        ArrayView<BinaryBuffer::value_type> buffer_view{buffer.data(), buffer.size()};
        auto read = file.read(buffer_view);
        T3D_ASSERT(size == read);
    }
    else
    {
        Log::error("Unable to open file: {0}", path);
    }

    return buffer;
}
