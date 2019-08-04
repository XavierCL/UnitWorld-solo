#pragma once

#include <unordered_set>

struct CollectionUtils
{
    template <typename Value, typename ValueHash = std::hash<Value>, typename ValueEqual = std::equal_to<Value>>
    std::unordered_set<Value, ValueHash, ValueEqual> toUnorderedSet(const std::vector<Value>& values)
    {
        return std::unordered_set<Value, ValueHash, ValueEqual>(values.begin(), values.end());
    }
};