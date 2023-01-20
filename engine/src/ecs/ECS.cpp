#include "ECS.h"

#include <RangeUtil.h>

namespace ecs
{

void EntityEventCallbackList::call(ecs::EntityFacade entity, const detail::ComponentMask& mask)
{
    for (auto& item : callbacks)
    {
        if (item.aspect.is_match(mask))
        {
            item.callback(entity);
        }
    }
}

void EntityEventCallbackList::add(int id, Aspect selector, EntityEventCallback callback)
{
    T3D_ASSERT(callback);
    callbacks.emplace_back(id, selector, callback);
}

void EntityEventCallbackList::remove(int id)
{
    for (int index = static_cast<int>(callbacks.size()) - 1; index >= 0; --index)
    {
        if (callbacks[index].id == id)
        {
            callbacks[index] = callbacks.back();
            callbacks.pop_back();
        }
    }
}

EntityFacade ECS::create_entity()
{
    T3D_ASSERT(first_free <= entities.size());

    if (first_free == entities.size())
    {
        entities.push_back(EntityID());
        entities.back().index = entities.size();
        entities.back().version = 0;
        component_masks.push_back(0);
    }

    T3D_ASSERT(!component_masks[first_free].any()); // Should not have any registered components
    auto& entity = entities[first_free];
    std::swap(entity.index, first_free);

    return EntityFacade(entity, this);
}

void ECS::destroy_entity(const EntityID& entity)
{
    if (!contains(entity))
    {
        T3D_FAIL("Double free");
        return;
    }

    destroyed_entities.insert(entity);
}

void ECS::cleanup_entities()
{
    for (auto& entity : destroyed_entities)
    {
        cleanup_entity(entity);
    }
    destroyed_entities.clear();
}

void ECS::cleanup_entity(const EntityID& entity)
{
    auto& stored_entity = entities[entity.index];
    if (entity == stored_entity)
    {
        destroy_components(entity);
        ++stored_entity.version;
        std::swap(stored_entity.index, first_free);
    }
}

void ECS::destroy_components(const EntityID& entity)
{
    auto old_mask = component_masks[entity.index];
    for (detail::ComponentID id = 0; id < detail::MaxComponentCount; ++id)
    {
        if (!old_mask[id])
        {
            continue;
        }

        T3D_ASSERT(active_components[id]); // Got component without any pool
        components[id].remove(entity);
    }
    component_masks[entity.index].reset();

    // Trigger callbacks based on old mask to enable detecting the component being removed
    on_component_removed_event.call({entity, this}, old_mask);
}

void* ECS::get_component_by_id(const EntityID& entity, detail::ComponentID id) const
{
    if(!active_components[id]) { return nullptr; }

    // We allow returning mutable memory so this method can be used by both const and non-const instances
    const auto& pool = components[id];
    return const_cast<ComponentPool&>(pool).get(entity);
}

void* ECS::add_component_by_id(const EntityID& entity, detail::ComponentID id, std::size_t size)
{
    auto& pool_location = components[id];
    if (!active_components[id])
    {
        active_components[id] = true;
        pool_location.init(size);
    }

    component_masks[entity.index].set(id, true);
    on_component_added_event.call(EntityFacade(entity, this), component_masks[entity.index]);
    return pool_location.add(entity);
}

bool ECS::try_remove_component(const EntityID& entity, detail::ComponentID id)
{
    if (!contains(entity)) { return false; }

    bool has_component = has_component_unsafe(entity, id);
    if (has_component)
    {
        remove_component(entity, id);
    }

    return has_component;
}

void ECS::remove_component(const EntityID& entity, detail::ComponentID id)
{
    if (!contains(entity)) { return; }

    T3D_ASSERT(active_components[id]); // Got component without any pool
    components[id].remove(entity);
    auto old_mask = component_masks[entity.index];
    component_masks[entity.index].set(id, false);

    // Trigger callbacks based on old mask to enable detecting the component being removed
    on_component_removed_event.call({entity, this}, old_mask);
}

EntityFacade ECS::find_first(const Aspect& aspect)
{
    EntityFacade found;
    for (std::size_t entity_index = 0; entity_index < component_masks.size(); ++entity_index)
    {
        const auto& mask = component_masks[entity_index];
        if (aspect.is_match(mask))
        {
            const auto& entity_id = entities[entity_index];
            found = get_facade(entity_id);
        }
    }
    return found;
}

ReadOnlyEntityFacade ECS::find_first(const Aspect& aspect) const
{
    ReadOnlyEntityFacade found;
    for (std::size_t entity_index = 0; entity_index < component_masks.size(); ++entity_index)
    {
        const auto& mask = component_masks[entity_index];
        if (aspect.is_match(mask))
        {
            const auto& entity_id = entities[entity_index];
            found = get_facade(entity_id);
        }
    }
    return found;
}

std::vector<EntityFacade> ECS::find_all(const Aspect& aspect)
{
    std::vector<EntityFacade> found;
    for (std::size_t entity_index = 0; entity_index < component_masks.size(); ++entity_index)
    {
        const auto& mask = component_masks[entity_index];
        if (!aspect.is_match(mask)) { continue; }

        const auto& entity_id = entities[entity_index];
        if (is_destroyed(entity_id)) { continue; }

        found.emplace_back(get_facade(entity_id));
    }
    return found;
}

std::vector<ReadOnlyEntityFacade> ECS::find_all(const Aspect& aspect) const
{
    std::vector<ReadOnlyEntityFacade> found;
    for (std::size_t entity_index = 0; entity_index < component_masks.size(); ++entity_index)
    {
        const auto& mask = component_masks[entity_index];
        if (!aspect.is_match(mask)) { continue; }

        const auto& entity_id = entities[entity_index];
        if (is_destroyed(entity_id)) { continue; }

        found.emplace_back(get_facade(entity_id));
    }
    return found;
}

}
