#pragma once

#include "Component.h"

namespace ecs
{

class Aspect
{
    Aspect() = default;
    Aspect(detail::ComponentMask required_components, detail::ComponentID primary_component)
        : primary_component(primary_component)
        , required_components{std::move(required_components)}
    {}

public:
    static Aspect all();

    template<typename ...ComponentType>
    static Aspect all_with();

    template<typename ...ComponentType>
    Aspect and_without();

    bool is_match(const detail::ComponentMask& mask) const;
    detail::ComponentID get_primary_component_id() { return primary_component; }

private:
    detail::ComponentID primary_component = 0;
    detail::ComponentMask required_components;
    detail::ComponentMask disallowed_components;
};

inline Aspect Aspect::all()
{
    return Aspect();
}

template<typename ...ComponentType>
inline Aspect Aspect::all_with()
{
    auto set = detail::get_mask_and_primary<ComponentType...>();
    return Aspect(set.mask, set.primary_component);
}

template<typename ...ComponentType>
inline Aspect Aspect::and_without()
{
    disallowed_components |= detail::get_mask<ComponentType...>();
    return *this;
}

inline bool Aspect::is_match(const detail::ComponentMask& mask) const
{
    const bool has_required = (required_components & mask) == required_components;
    const bool has_disallowed = (disallowed_components & mask).any();
    return has_required && !has_disallowed;
}

}
