#pragma once

#include "Hash.hpp"
#include <unordered_set>

template <typename _Type>
class Set: public std::unordered_set<_Type, Hash<_Type>>
{};
