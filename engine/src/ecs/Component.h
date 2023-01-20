#pragma once

#include <diag/Assert.h>

#include <bitset>

namespace ecs { namespace detail
{

using ComponentID = unsigned;
static const unsigned MaxComponentCount = 128;
using ComponentMask = std::bitset<MaxComponentCount>;
struct ComponentMaskPrimary
{
    ComponentID primary_component = 0;
    ComponentMask mask;
};

ComponentID pop_next_component_id();

template<typename ComponentType>
inline ComponentID get_component_id()
{
    static const ComponentID component_id = pop_next_component_id();
    T3D_ASSERT(component_id < MaxComponentCount);
    return component_id;
}

template<typename ...ComponentType>
inline ComponentMaskPrimary get_mask_and_primary()
{
    std::initializer_list<ComponentID> component_ids = { get_component_id<ComponentType>()... };
    ComponentMaskPrimary set;

    if (component_ids.size())
    {
        set.primary_component = *component_ids.begin();
    }

    for (ComponentID id : component_ids)
    {
        set.mask.set(id);
    }
    return set;
}

template<typename ...ComponentType>
inline ComponentMask get_mask()
{
    ComponentMaskPrimary set = get_mask_and_primary<ComponentType...>();
    return set.mask;
}

}}
