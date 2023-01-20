#include "StringTools.h"

#include <cstring>
#include <cstdarg>

std::vector<StringView> stringtools::wrap_text(const StringView& line, std::size_t max_length)
{
    std::size_t last_whitespace = 0;
    std::size_t last_start = 0;
    std::vector<StringView> lines;
    for (std::size_t index = 0; index < line.get_size(); ++index)
    {
        if (line[index] == ' ')
        {
            last_whitespace = index;
        }
        if ((index - last_start) > max_length)
        {
            lines.push_back(line.slice(last_start, last_whitespace));
            last_start = last_whitespace + 1;
        }
    }

    if (last_start != line.get_size())
    {
        lines.push_back(line.slice_from(last_start));
    }

    return lines;
}

StringView stringtools::sprintf(const char* fmt, ...)
{
    static const std::size_t MAX_BUFFER_SIZE = 1024;
    static char buffer[MAX_BUFFER_SIZE];

    std::va_list args;
    va_start(args, fmt);
    int num_char = std::vsnprintf(buffer, MAX_BUFFER_SIZE, fmt, args);
    va_end(args);
    std::size_t buffer_size = MAX_BUFFER_SIZE;
    if (num_char < 0)
    {
        buffer[0] = 0;
        buffer_size = 0;
    }
    else if (static_cast<unsigned>(num_char) < MAX_BUFFER_SIZE)
    {
        buffer_size = static_cast<std::size_t>(num_char);
    }
    return {buffer, buffer_size};
}
