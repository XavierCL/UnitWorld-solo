#pragma once

#include <cstdint>
#include <memory>

template <typename _Type>
class PointerHash
{
public:

	size_t operator()(_Type const * const &hashing) const
	{
		return reinterpret_cast<uintptr_t>(hashing) % static_cast<uintptr_t>(sizeof(size_t));
	}
};

template <typename _Type>
class SharedPointerHash
{
public:

	size_t operator()(const std::shared_ptr<_Type> &hashing) const
	{
		return PointerHash<_Type>().operator()(hashing.get());
	}
};

template <typename _Type>
class WeakPointerHash
{
public:

	size_t operator()(const std::weak_ptr<_Type> &hashing) const
	{
		return SharedPointerHash<_Type>().operator()(hashing.lock());
	}
};
