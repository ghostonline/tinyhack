#pragma once

#include "Ref.h"

#include <diag/Assert.h>

#include <vector>
#include <algorithm>

template<typename ItemType>
class Pool
{
public:
    using RefType = Ref<ItemType>;

    struct VersionedItem
    {
        template<class... Args>
        VersionedItem(unsigned index, unsigned version, Args&&... __args) : ref{ index, version }, item(std::forward<Args>(__args)...) {}

        Ref<ItemType> ref;
        ItemType item;
        bool free = false;
    };

    using PoolStorage = std::vector<VersionedItem>;

    Pool(std::size_t size)
    {
        data.reserve(size);
    }

    template<class... Args>
    RefType add(Args&&... args);

    void remove(const RefType& ref);

    ItemType* get(const RefType& ref);
    const ItemType* get(const RefType& ref) const;

    typename PoolStorage::const_iterator begin() const { return data.begin(); }
    typename PoolStorage::const_iterator end() const { return data.end(); }

private:
    PoolStorage data;
};

template<typename ItemType>
template<class... Args>
typename Pool<ItemType>::RefType Pool<ItemType>::add(Args&&... args)
{
    T3D_ASSERT(data.capacity() > data.size());
    auto free_item = std::find_if(std::begin(data), std::end(data), [](const VersionedItem& item) { return item.free; });
    Ref<ItemType> ref;
    if (free_item == std::end(data))
    {
        data.emplace_back(static_cast<unsigned>(data.size()), 0, std::forward<Args>(args)...);
        ref = data.back().ref;
    }
    else
    {
        ref = free_item->ref;
        free_item->free = false;
        free_item->item = ItemType(std::forward<Args>(args)...);
    }
    return ref;
}

template<typename ItemType>
ItemType* Pool<ItemType>::get(const RefType& ref)
{
    T3D_ASSERT(ref.index() < data.size());
    auto& item = data[ref.index()];
    if (item.ref.version() == ref.version())
    {
        return &item.item;
    }
    else
    {
        return nullptr;
    }
}

template<typename ItemType>
const ItemType* Pool<ItemType>::get(const RefType& ref) const
{
    T3D_ASSERT(ref.index() < data.size());
    auto& item = data[ref.index()];
    if (item.ref.version() == ref.version())
    {
        return &item.item;
    }
    else
    {
        return nullptr;
    }
}

template<typename ItemType>
void Pool<ItemType>::remove(const RefType& ref)
{
    T3D_ASSERT(ref.index() < data.size());
    auto& item = data[ref.index()];
    T3D_ASSERT(!item.free);
    item.free = true;
    item.ref = Ref<ItemType>(ref.index(), ref.version() + 1);
}
