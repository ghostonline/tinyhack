#pragma once

#include <ds/StringView.h>

enum class LangID
{
#define LANG_ENTRY(id, en_str) id,
#include "LangData.h"
#undef LANG_ENTRY
};


class Lang
{
public:
    static const char* get(LangID id);
    static const StringView& get_view(LangID id);
};
