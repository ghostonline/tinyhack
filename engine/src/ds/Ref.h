#pragma once

template<typename T>
class Ref
{
public:
    Ref() = default;
    Ref(unsigned index, unsigned version)
    : _index(index), _version(version)
    {}

    unsigned index() const { return _index; }
    unsigned version() const { return _version; }
    operator bool() const { return _index != static_cast<unsigned>(-1) && _version != static_cast<unsigned>(-1); }

    bool operator==(Ref& rhs) const { return _index == rhs.index() && _version == rhs.version(); }
    bool operator!=(Ref& rhs) const { return _index != rhs.index() || _version != rhs.version(); }

    static const Ref Null;

private:
    unsigned _index = static_cast<unsigned>(-1);
    unsigned _version = static_cast<unsigned>(-1);
};

template<typename T>
const Ref<T> Ref<T>::Null = Ref<T>();
