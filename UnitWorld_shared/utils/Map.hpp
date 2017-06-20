#pragma once

#include <unordered_map>

#include "Hash.hpp"

template <typename _keyType, typename _valueType>
class Map: public unordered_map<_keyType, _valueType, Hash<_keyType>>
{};