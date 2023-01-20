#pragma once

#include <Random.h>
#include <ds/Size2.h>

#include <utility>

namespace util
{

template<typename ContainerType>
void shuffle(ContainerType& container, Random& rng)
{
    const std::size_t container_size = container.size();
    if (container_size == 0) { return; }

    for (std::size_t index = container_size - 1; index > 0; --index)
    {
        std::size_t other_index = rng.next(static_cast<int>(index + 1));
        std::swap(container[index], container[other_index]);
    }
}

template<typename Container>
inline std::size_t choose_index(const Container& container, Random& rng)
{
    const int size = static_cast<int>(container.size());
    const int index = rng.next(size);
    return static_cast<std::size_t>(index);
}

template<typename Container>
inline typename Container::reference& choose(Container& container, Random& rng)
{
    std::size_t index = choose_index(container, rng);
    return container[index];
}

template<typename T>
inline void choose(const Size2<T>& area, Random& rng, T& x, T& y)
{
    x = rng.next(area.width);
    y = rng.next(area.height);
}

}
