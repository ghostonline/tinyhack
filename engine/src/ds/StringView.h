#pragma once

#include "ArrayView.h"
#include <string>

class StringView : public ArrayView<const char>
{
public:
    using ArrayView::ArrayView;

    StringView() {}
    StringView(const char* ptr) : ArrayView(ptr, std::strlen(ptr)) {}
    StringView(const std::string& str) : ArrayView(str.c_str(), str.size()) {}
    StringView(const ArrayView<const char>& view) : ArrayView(view.get_ptr(), view.get_size()) {}

    StringView& operator=(const ArrayView<const char>&);

    std::string to_string() const { return std::string{get_ptr(), get_size()}; }
    operator std::string() const { return to_string(); }
};

inline StringView& StringView::operator=(const ArrayView<const char>& rhs)
{
    *this = StringView(rhs.get_ptr(), rhs.get_size());
    return *this;
}

inline bool operator==(const StringView& lhs, const StringView& rhs)
{
    if (&lhs == &rhs) { return true; }
    if (lhs.get_size() != rhs.get_size()) { return false; }
    if (lhs.get_ptr() == rhs.get_ptr()) { return true; }
    return std::strncmp(lhs.get_ptr(), rhs.get_ptr(), rhs.get_size()) == 0;
}

inline bool operator!=(const StringView& lhs, const StringView& rhs)
{
    return (lhs == rhs) == false;
}
