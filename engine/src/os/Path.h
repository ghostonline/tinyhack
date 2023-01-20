#pragma once

#include <string>
#include <ds/StringView.h>

class Path
{
public:
    struct Components
    {
        StringView base;
        StringView filename;
    };

    Path() {}
    Path(std::initializer_list<StringView> components);
    explicit Path(const StringView& full_path) : full_path(full_path.get_ptr(), full_path.get_size()) {}

    static Path concat(const Path& a, const Path& b);

    const StringView to_view() const { return { full_path.data(), full_path.size() }; }
    const char* to_cstr() const { return full_path.c_str(); }

    Components split_filename() const;
    StringView get_filename() const { return split_filename().filename; }
    StringView get_base() const { return split_filename().base; }

private:
    std::string full_path;

    static const char SEPARATOR;
};
