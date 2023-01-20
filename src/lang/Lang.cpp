#include "Lang.h"

struct LangRecord
{
    StringView english;
};

static const LangRecord StringTable[] = {
#define LANG_ENTRY(id, en_str) { en_str },
#include "LangData.h"
#undef LANG_ENTRY
};

const char* Lang::get(LangID id)
{
    return get_view(id).get_ptr();
}

const StringView& Lang::get_view(LangID id)
{
    return StringTable[static_cast<std::size_t>(id)].english;
}
