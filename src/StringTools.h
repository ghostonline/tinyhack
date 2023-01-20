#pragma once

#include <ds/StringView.h>
#include <vector>

namespace stringtools
{
    std::vector<StringView> wrap_text(const StringView& line, std::size_t max_length);
    StringView sprintf(const char* fmt, ...);
}
