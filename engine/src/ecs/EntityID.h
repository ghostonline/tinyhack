#pragma once

#include <cstddef>
#include <functional>

namespace ecs
{

struct EntityID
{
    EntityID() = default;
    explicit EntityID(std::size_t index) : index(index) {}
    EntityID(std::size_t index, unsigned version) : index(index), version(version) {}

    operator bool() const { return version != static_cast<unsigned>(-1); }

    std::size_t index = 0;
    unsigned version = static_cast<unsigned>(-1);
};

static const EntityID NullEntityID = EntityID();

inline bool operator==(const EntityID& lhs, const EntityID& rhs)
{
    return lhs.index == rhs.index && lhs.version == rhs.version;
}

inline bool operator!=(const EntityID& lhs, const EntityID& rhs)
{
    return (lhs == rhs) == false;
}

}


namespace std
{

template<> struct hash<ecs::EntityID>
{
    typedef ecs::EntityID argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& entity) const noexcept
    {
        result_type const h1 ( std::hash<std::size_t>{}(entity.index) );
        result_type const h2 ( std::hash<unsigned>{}(entity.version) );
        return h1 ^ (h2 << 1); // or use boost::hash_combine (see Discussion)
    }
};

}
