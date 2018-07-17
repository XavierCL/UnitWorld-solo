#pragma once

#include <functional>
#include <cstdint>
#include <memory>

template <typename _Type>
class Hash
{
public:

	const size_t operator()(const _Type &hashed) const
	{
		return reinterpret_cast<uintptr_t>(hashed) % static_cast<uintptr_t>(sizeof(size_t));
	}
};

template <typename _Type>
class Hash<std::shared_ptr<_Type>>
{
public:
	size_t operator()(const std::shared_ptr<_Type>& hashed) const
	{
		return _underlyingHash(hashed);
	}

	static const std::hash<std::shared_ptr<_Type>> _underlyingHash;
};

template <typename _Type>
const std::hash<std::shared_ptr<_Type>> Hash<std::shared_ptr<_Type>>::_underlyingHash;