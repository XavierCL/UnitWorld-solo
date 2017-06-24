#pragma once

#include <string>
#include <functional>

template <typename _Type>
class Hash
{
public:
	size_t operator()(const _Type &&element) const
	{
		return _innerHash(std::string((char*)&element, sizeof(_Type)));
	}
	size_t operator()(const _Type* &&element) const
	{
		return _innerHash(std::string((char*)element, sizeof(_Type*)));
	}
private:
	static const std::hash<std::string> _innerHash;
};

//template <typename _Type> const std::hash<std::string> Hash<_Type>::_innerHash;