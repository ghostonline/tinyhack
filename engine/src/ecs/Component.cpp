#include "Component.h"

ecs::detail::ComponentID ecs::detail::pop_next_component_id()
{
    static ecs::detail::ComponentID next_component_id = 0;
    return next_component_id++;
}
