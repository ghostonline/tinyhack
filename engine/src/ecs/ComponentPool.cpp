#include "ComponentPool.h"

void ecs::ComponentPool::init(std::size_t new_component_size)
{
    component_size = new_component_size;
    static const std::size_t storage_size = sizeof(StorageType);
    // Round to nearest multiple of StorageType
    component_memory_size = ((component_size + storage_size - 1) / storage_size) * storage_size;
    component_memory_blocks = component_memory_size / storage_size;
    T3D_ASSERT(component_size > 0);
    T3D_ASSERT(component_memory_size >= component_size);

    component_count = 0;
    handles.clear(); handles.reserve(0);
    data_owners.clear(); data_owners.reserve(0);
    pool.clear(); pool.reserve(0);
}

void* ecs::ComponentPool::get(const EntityID& entity)
{
    auto redirect = handles.find(entity);
    if (redirect != end(handles))
    {
        return get_component_memory(redirect->second);
    }
    else
    {
        return nullptr;
    }
}

void* ecs::ComponentPool::add(const EntityID& entity)
{
    std::size_t component_index = 0;
    auto redirect = handles.find(entity);
    if (redirect == end(handles))
    {
        component_index = component_count;
        update_pool_size(component_count + 1);
        data_owners.emplace_back(entity);
        handles.emplace(entity, component_index);
    }
    else
    {
        T3D_FAIL("Component already present");
        component_index = redirect->second;
        return &pool[redirect->second];
    }
    return get_component_memory(component_index);
}

void ecs::ComponentPool::remove(const EntityID& entity)
{
    auto redirect = handles.find(entity);
    if(redirect == end(handles))
    {
        T3D_FAIL("Double free");
        return;
    }
    auto data_index = redirect->second;
    void* removed_component = get_component_memory(data_index);

    // Copy last component data to removed component location
    if (component_count > 1)
    {
        void* last_component = get_component_memory(component_count - 1);
        memcpy(removed_component, last_component, component_size);

        auto last_data_owner = data_owners.back();
        data_owners[data_index] = last_data_owner;

        // Update last component redirect
        auto last_data_owner_redirect = handles.find(last_data_owner);
        if (last_data_owner_redirect == end(handles))
        {
            T3D_FAIL("Nonexisting owner");
        }
        else
        {
            last_data_owner_redirect->second = data_index;
        }
    }

    // Remove component data entry
    update_pool_size(component_count - 1);
    data_owners.pop_back();

    // Invalidate handle
    handles.erase(redirect);
}
