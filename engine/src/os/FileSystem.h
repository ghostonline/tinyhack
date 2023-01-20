#pragma once

#include <vector>

class Path;

using TextBuffer = std::vector<char>;
using BinaryBuffer = std::vector<unsigned char>;

namespace filesystem
{
    Path get_resource_path(const Path& path);

    TextBuffer load_text_file(const Path& path);
    BinaryBuffer load_binary_file(const Path& path);
}
