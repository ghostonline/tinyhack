#pragma once

#include "Aspect.h"
#include "EntityID.h"
#include "Component.h"
#include "ComponentPool.h"

#include <diag/Assert.h>

#include <vector>
#include <tuple>
#include <set>

namespace ecs
{

class ECS;

class ReadOnlyEntityFacade
{
public:
    ReadOnlyEntityFacade() = default;
    ReadOnlyEntityFacade(const EntityID& entity_id, const ECS* entities) : _entity_id(entity_id), entities(entities) {}
    ReadOnlyEntityFacade(EntityID&& entity_id, const ECS* entities) : _entity_id(std::move(entity_id)), entities(entities) {}

    template<typename ComponentType>
    bool has_component() const;
    template<typename ComponentType>
    const ComponentType* get_component() const;
    template<typename ...ComponentType>
    std::tuple<const ComponentType*...> get_components() const;

    const EntityID& entity_id() const { return _entity_id; }

    operator bool() const { return _entity_id && entities != nullptr; }

private:
    EntityID _entity_id = EntityID();
    const ECS* entities = nullptr;
};

class EntityFacade
{
public:
    EntityFacade() = default;
    EntityFacade(const EntityID& entity_id, ECS* entities) : _entity_id(entity_id), entities(entities) {}
    EntityFacade(EntityID&& entity_id, ECS* entities) : _entity_id(std::move(entity_id)), entities(entities) {}

    void destroy();

    template<typename ComponentType, typename ...Args>
    ComponentType* add_component(Args&& ...args);
    template<typename ComponentType, typename ...Args>
    ComponentType* ensure_component(Args&& ...args);
    template<typename ComponentType>
    void remove_component();
    template<typename ComponentType>
    bool try_remove_component();
    template<typename ComponentType>
    bool has_component() const;
    template<typename ComponentType>
    ComponentType* get_component();
    template<typename ComponentType>
    ComponentType* get_component() const;
    template<typename ...ComponentType>
    std::tuple<ComponentType*...> get_components();
    template<typename ...ComponentType>
    std::tuple<const ComponentType*...> get_components() const;

    const EntityID& entity_id() const { return _entity_id; }
    operator ReadOnlyEntityFacade() const { return ReadOnlyEntityFacade(_entity_id, entities); }

    operator bool() const { return _entity_id && entities != nullptr; }

private:
    EntityID _entity_id = EntityID();
    ECS* entities = nullptr;
};

static const EntityFacade NullEntity = EntityFacade();
using EntityEventCallback = std::function<void(ecs::EntityFacade)>;

struct EntityEventCallbackList
{
    struct Item
    {
        Item(int id, Aspect aspect, EntityEventCallback callback) : id(id), aspect(aspect), callback(callback) {}

        int id;
        Aspect aspect;
        EntityEventCallback callback;
    };

    void call(ecs::EntityFacade entity, const detail::ComponentMask& mask);
    void add(int id, Aspect selector, EntityEventCallback callback);
    void remove(int id);

    std::vector<Item> callbacks;
};

class ECS
{
public:
    EntityFacade create_entity();
    void destroy_entity(const EntityID& entity);
    void cleanup_entities();

    template<typename ComponentType, typename ...Args>
    ComponentType* add_component(const EntityID& entity, Args&& ...args);
    template<typename ComponentType, typename ...Args>
    ComponentType* ensure_component(const EntityID& entity, Args&& ...args);
    template<typename ComponentType>
    void remove_component(const EntityID& entity);
    template<typename ComponentType>
    bool try_remove_component(const EntityID& entity);
    template<typename ComponentType>
    bool has_component(const EntityID& entity);
    template<typename ComponentType>
    ComponentType* get_component(const EntityID& entity);
    template<typename ComponentType>
    const ComponentType* get_component(const EntityID& entity) const;
    template<typename ...ComponentType>
    std::tuple<ComponentType*...> get_components(const EntityID& entity);
    template<typename ...ComponentType>
    std::tuple<const ComponentType*...> get_components(const EntityID& entity) const;

    std::vector<EntityFacade> find_all(const Aspect& aspect);
    std::vector<ReadOnlyEntityFacade> find_all(const Aspect& aspect) const;

    template<typename ...ComponentType>
    std::vector<EntityFacade> find_all();
    template<typename ...ComponentType>
    std::vector<ReadOnlyEntityFacade> find_all() const;

    EntityFacade find_first(const Aspect& aspect);
    ReadOnlyEntityFacade find_first(const Aspect& aspect) const;
    template<typename ...ComponentType>
    EntityFacade find_first();
    template<typename ...ComponentType>
    ReadOnlyEntityFacade find_first() const;

    EntityEventCallbackList on_component_added_event;
    EntityEventCallbackList on_component_removed_event;

private:
    void cleanup_entity(const EntityID& entity);
    bool contains(const EntityID& entity) const;
    bool has_component_unsafe(const EntityID& entity, detail::ComponentID id) const;
    bool is_destroyed(const EntityID& entity) const;
    void destroy_components(const EntityID& entity);
    void* get_component_by_id(const EntityID& entity, detail::ComponentID id) const;
    void* add_component_by_id(const EntityID& entity, detail::ComponentID id, std::size_t size);
    void remove_component(const EntityID& entity, detail::ComponentID id);
    bool try_remove_component(const EntityID& entity, detail::ComponentID id);
    EntityFacade get_facade(const EntityID& entity);
    ReadOnlyEntityFacade get_facade(const EntityID& entity) const;

    std::size_t first_free = 0;
    std::vector<EntityID> entities;
    std::set<EntityID> destroyed_entities;
    std::vector<detail::ComponentMask> component_masks;
    bool active_components[detail::MaxComponentCount];
    ComponentPool components[detail::MaxComponentCount];
};

inline bool ECS::contains(const EntityID& entity) const
{
    return entity.index < entities.size() && entities[entity.index] == entity && !is_destroyed(entity);
}

inline bool ECS::has_component_unsafe(const EntityID& entity, detail::ComponentID id) const
{
    return component_masks[entity.index][id];
}

inline bool ECS::is_destroyed(const EntityID& entity) const
{
    return destroyed_entities.find(entity) != end(destroyed_entities);
}

template<typename ComponentType, typename ...Args>
ComponentType* ECS::add_component(const EntityID& entity, Args&& ...args)
{
    T3D_ASSERT(contains(entity));
    auto component_id = detail::get_component_id<ComponentType>();
    void* storage = add_component_by_id(entity, component_id, sizeof(ComponentType));
    return new (storage) ComponentType(std::forward<Args>(args)...);
}

template<typename ComponentType, typename ...Args>
ComponentType* ECS::ensure_component(const EntityID& entity, Args&& ...args)
{
    T3D_ASSERT(contains(entity));
    void* component = nullptr;
    auto component_id = detail::get_component_id<ComponentType>();
    if (has_component_unsafe(entity, component_id))
    {
        component = get_component_by_id(entity, component_id);
    }
    else
    {
        component = add_component_by_id(entity, component_id, sizeof(ComponentType));
        new (component) ComponentType(std::forward<Args>(args)...);
    }
    return static_cast<ComponentType*>(component);
}

template<typename ComponentType>
inline void ECS::remove_component(const EntityID& entity)
{
    auto component_id = detail::get_component_id<ComponentType>();
    remove_component(entity, component_id);
}

template<typename ComponentType>
inline bool ECS::try_remove_component(const EntityID& entity)
{
    auto component_id = detail::get_component_id<ComponentType>();
    return try_remove_component(entity, component_id);
}

template<typename ComponentType>
bool ECS::has_component(const EntityID& entity)
{
    auto component_id = detail::get_component_id<ComponentType>();
    return contains(entity) && has_component_unsafe(entity, component_id);
}

template<typename ComponentType>
ComponentType* ECS::get_component(const EntityID& entity)
{
    auto component_id = detail::get_component_id<ComponentType>();
    return static_cast<ComponentType*>(get_component_by_id(entity, component_id));
}

template<typename ComponentType>
const ComponentType* ECS::get_component(const EntityID& entity) const
{
    auto component_id = detail::get_component_id<ComponentType>();
    return static_cast<const ComponentType*>(get_component_by_id(entity, component_id));
}

template<typename ...ComponentType>
inline std::tuple<ComponentType*...> ECS::get_components(const EntityID& entity)
{
    return std::make_tuple(get_component<ComponentType>(entity)...);
}

template<typename ...ComponentType>
inline std::tuple<const ComponentType*...> ECS::get_components(const EntityID& entity) const
{
    return std::make_tuple(get_component<ComponentType>(entity)...);
}

template<typename ...ComponentType>
inline std::vector<EntityFacade> ECS::find_all()
{
    return find_all(Aspect::all_with<ComponentType...>());
}

template<typename ...ComponentType>
inline std::vector<ReadOnlyEntityFacade> ECS::find_all() const
{
    return find_all(Aspect::all_with<ComponentType...>());
}

template<typename ...ComponentType>
inline EntityFacade ECS::find_first()
{
    return find_first(Aspect::all_with<ComponentType...>());
}

template<typename ...ComponentType>
inline ReadOnlyEntityFacade ECS::find_first() const
{
    return find_first(Aspect::all_with<ComponentType...>());
}

inline EntityFacade ECS::get_facade(const EntityID& entity)
{
    return EntityFacade(entity, this);
}

inline ReadOnlyEntityFacade ECS::get_facade(const EntityID& entity) const
{
    return ReadOnlyEntityFacade(entity, this);
}

// EntityFacade methods (needs to come after ECS definitions to function)

template<typename ComponentType>
inline bool ReadOnlyEntityFacade::has_component() const
{
    return entities->has_component<ComponentType>(_entity_id);
}

template<typename ComponentType>
inline const ComponentType* ReadOnlyEntityFacade::get_component() const
{
    return entities->get_component<ComponentType>(_entity_id);
}

template<typename ...ComponentType>
inline std::tuple<const ComponentType*...> ReadOnlyEntityFacade::get_components() const
{
    return entities->get_components<ComponentType...>(_entity_id);
}

inline void EntityFacade::destroy()
{
    entities->destroy_entity(_entity_id);
}

template<typename ComponentType>
inline bool EntityFacade::has_component() const
{
    return entities->has_component<ComponentType>(_entity_id);
}

template<typename ComponentType, typename ...Args>
inline ComponentType* EntityFacade::add_component(Args&& ...args)
{
    return entities->add_component<ComponentType>(_entity_id, std::forward<Args>(args)...);
}

template<typename ComponentType, typename ...Args>
inline ComponentType* EntityFacade::ensure_component(Args&& ...args)
{
    return entities->ensure_component<ComponentType>(_entity_id, std::forward<Args>(args)...);
}

template<typename ComponentType>
inline void EntityFacade::remove_component()
{
    entities->remove_component<ComponentType>(_entity_id);
}

template<typename ComponentType>
inline bool EntityFacade::try_remove_component()
{
    return entities->try_remove_component<ComponentType>(_entity_id);
}

template<typename ComponentType>
inline ComponentType* EntityFacade::get_component()
{
    return entities->get_component<ComponentType>(_entity_id);
}

template<typename ComponentType>
inline ComponentType* EntityFacade::get_component() const
{
    return entities->get_component<ComponentType>(_entity_id);
}

template<typename ...ComponentType>
inline std::tuple<ComponentType*...> EntityFacade::get_components()
{
    return entities->get_components<ComponentType...>(_entity_id);
}

template<typename ...ComponentType>
inline std::tuple<const ComponentType*...> EntityFacade::get_components() const
{
    return entities->get_components<ComponentType...>(_entity_id);
}

}
