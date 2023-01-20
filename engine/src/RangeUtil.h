#pragma once

#include <algorithm>

namespace range
{
    template<typename RangeType, typename OutputIt>
    void move(RangeType& range, OutputIt out)
    {
        move(std::begin(range), std::end(range), out);
    }

    template<typename RangeType, typename OutputIt>
    void copy(const RangeType& range, OutputIt out)
    {
        copy(std::begin(range), std::end(range), out);
    }

    template<typename RangeType, typename OutputIt, typename PredicateType>
    void copy_if(const RangeType& range, OutputIt out, const PredicateType& predicate)
    {
        copy_if(std::begin(range), std::end(range), out, predicate);
    }

    template<typename RangeType, typename OutputIt, typename PredicateType>
    void transform(const RangeType& range, OutputIt out, const PredicateType& predicate)
    {
        transform(std::begin(range), std::end(range), out, predicate);
    }

    template<typename RangeType, typename OutputIt>
    void copy_ptr(const RangeType& range, OutputIt out)
    {
        const auto GetPtr = [](const typename RangeType::value_type& item)
        {
            return const_cast<typename RangeType::value_type*>(&item);
        };
        transform(range, out, GetPtr);
    }

    template<typename RangeType, typename PredicateType>
    void for_each(const RangeType& range, const PredicateType& predicate)
    {
        for_each(std::begin(range), std::end(range), predicate);
    }

    template<typename RangeType, typename PredicateType>
    bool any_of(const RangeType& range, const PredicateType& predicate)
    {
        return any_of(std::begin(range), std::end(range), predicate);
    }

    template<typename RangeType, typename PredicateType>
    typename RangeType::const_iterator find_if(const RangeType& range, const PredicateType& predicate)
    {
        return find_if(std::begin(range), std::end(range), predicate);
    }

    template<typename RangeType, typename PredicateType>
    typename RangeType::iterator find_if(RangeType& range, const PredicateType& predicate)
    {
        return find_if(std::begin(range), std::end(range), predicate);
    }

    template<typename RangeType, typename ElementType>
    typename RangeType::const_iterator find(const RangeType& range, ElementType& element)
    {
        return find(std::begin(range), std::end(range), element);
    }

    template<typename RangeType, typename ElementType>
    bool contains(const RangeType& range, ElementType& element)
    {
        return find(range, element) != range.end();
    }

    template<typename RangeType, typename CompareType>
    typename RangeType::iterator min_element(RangeType& range, CompareType compare)
    {
        return min_element(begin(range), end(range), compare);
    }

    template<typename RangeType, typename CompareType>
    typename RangeType::const_iterator min_element(const RangeType& range, CompareType compare)
    {
        return min_element(begin(range), end(range), compare);
    }

    template<typename Type, typename PredicateType>
    void erase_if(std::vector<Type>& range, const PredicateType& predicate)
    {
        int last_index = static_cast<int>(range.size() - 1);
        for (int index = last_index; index >= 0; --index)
        {
            if (predicate(range[index]))
            {
                range[index] = std::move(range[last_index]);
                range.pop_back();
                --last_index;
            }
        }
    }
}
