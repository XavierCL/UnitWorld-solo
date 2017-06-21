#pragma once

#include "Hash.hpp"
#include <unordered_set>

template <typename _ty>
class Set: public std::unordered_set<_ty, Hash<_ty>>
{};
