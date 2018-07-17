#pragma once

#include "Hash.hpp"

#include <unordered_map>

template <typename _KeyType, typename _ValueType>
class Map: public std::unordered_map<_KeyType, _ValueType, Hash<_KeyType>>
{};