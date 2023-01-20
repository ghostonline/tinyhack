#pragma once

#include "EntityID.h"

#include <diag/Assert.h>
#include <ds/ArrayView.h>

#include <cstddef>
#include <vector>
#include <unordered_map>

namespace ecs
{

class ComponentPool
{
    // Use memory type that will align properly for all possible types
    using StorageType = std::max_align_t;

public:
    ComponentPool() = default;
    ComponentPool(std::size_t component_size) { init(component_size); }

    void init(std::size_t new_component_size);

    void* get(const EntityID& entity);
    void* add(const EntityID& entity);
    void remove(const EntityID& entity);

    ArrayView<const EntityID> get_owners() const { return {data_owners.data(), data_owners.size()}; }

private:
    std::size_t get_component_offset(std::size_t component_index) const { return component_index * component_memory_blocks; }
    void* get_component_memory(std::size_t component_index) { return pool.data() + get_component_offset(component_index); }
    void update_pool_size(std::size_t new_component_count);

    std::size_t component_size = 0;
    std::size_t component_memory_size = 0;
    std::size_t component_memory_blocks = 0;
    std::size_t component_count = 0;

    std::unordered_map<EntityID, std::size_t> handles;
    std::vector<EntityID> data_owners;
    std::vector<StorageType> pool;
};

inline void ComponentPool::update_pool_size(std::size_t new_component_count)
{
    component_count = new_component_count;
    pool.resize(component_count * component_memory_blocks);
}

}
