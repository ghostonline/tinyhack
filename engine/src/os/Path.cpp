#include "Path.h"

#include <vector>

#if defined(_WIN32)
const char Path::SEPARATOR = '\\';
#else
const char Path::SEPARATOR = '/';
#endif

Path::Path(std::initializer_list<StringView> components)
{
    StringView::size_type total_size = 0;
    for (const auto& component : components)
    {
        total_size += component.get_size() + 1;
    }

    std::vector<char> path(total_size);
    StringView::size_type current_size = 0;
    for (const auto& component : components)
    {
        memcpy(path.data() + current_size, component.get_ptr(), component.get_size());
        current_size += component.get_size();
        path[current_size] = SEPARATOR;
        ++current_size;
    }

    if (current_size) { --current_size; } // Strip remaining separator

    full_path.assign(path.data(), current_size);
}

Path Path::concat(const Path& a, const Path& b)
{
    std::vector<char> new_path(a.full_path.size() + b.full_path.size() + 1);
    std::size_t current_idx = 0;
    memcpy(new_path.data() + current_idx, a.full_path.data(), a.full_path.size());
    current_idx += a.full_path.size();
    new_path[current_idx] = SEPARATOR;
    current_idx += 1;
    memcpy(new_path.data() + current_idx, b.full_path.data(), b.full_path.size());
    current_idx += b.full_path.size();
    return Path({new_path.data(), new_path.size()});
}

Path::Components Path::split_filename() const
{
    Components components;
    auto idx = full_path.find_last_of(SEPARATOR);
    if (idx == std::string::npos)
    {
        components.filename = to_view();
    }
    else
    {
        auto start = idx + 1;
        auto size = full_path.size() - start;
        components.filename = StringView{full_path.data() + start, size};
        components.base = StringView{full_path.data(), idx};
    }
    return components;
}
