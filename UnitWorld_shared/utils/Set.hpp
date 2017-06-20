#pragma once

#include "Hash.hpp"

#include <unordered_set>

using namespace std;

template <typename _ty>
class Set: public unordered_set<_ty, Hash<_ty>>
{};
