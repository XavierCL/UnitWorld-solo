#pragma once

#include "Option.hpp"
#include "FastMath.h"

#include <vector>

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
class FixedUnorderedMap
{
public:
	FixedUnorderedMap(const size_t& minCapacity):
		_container(FastMath::nextOrSamePrime(minCapacity), Option<std::pair<_KeyType, _ValueType>>::None),
		_size(0)
	{}

	FixedUnorderedMap(const FixedUnorderedMap<_KeyType, _ValueType, _KeyHashType> &other):
		_container(other._size, Option<std::pair<_KeyType, _ValueType>>::None),
		_size(other._size)
	{
		size_t currentSize = 0;
		size_t currentIndex = 0;
		while (currentSize < _size)
		{
			if (other._container[currentIndex].defined())
			{
				_container[currentIndex] = other._container[currentIndex];
				++currentSize;
			}
			++currentIndex;
		}
	}

	Option<_ValueType> get(const _KeyType& key) const
	{
		return _container[_hasher(key) % _container.size()].flatMap<_ValueType>([&key](const std::shared_ptr<std::pair<_KeyType, _ValueType>>& element) {
			if (element->first == key)
			{
				return Option<_ValueType>(element.second);
			}
			else
			{
				return Option<_ValueType>();
			}
		});
	}

	void set(const _KeyType& key, const _ValueType& value)
	{
		const size_t keyIndex = _hasher(key) % _capacity;
		if (_container[keyIndex].isEmpty())
		{
			++_size;
		}
		_container[keyIndex] = Option<std::pair<_KeyType, _ValueType>>(std::make_pair(key, value));
	}

	void remove(const _KeyType& key)
	{
		const size_t keyIndex = _hasher(key) % _capacity;
		return _container[keyIndex].foreach([&key, this, &keyIndex](const std::shared_ptr<std::pair<_KeyType, _ValueType>>& element) {
			if (element->first == key)
			{
				_container[keyIndex] = Option<std::pair<_KeyType, _ValueType>>::None;
				--_size;
			}
		});
	}

	size_t size() const
	{
		return _size;
	}

	void clear()
	{
		for (size_t i = 0; i < _capacity; ++i)
		{
			_container[i] = Option<std::pair<_KeyType, _ValueType>>::None;
		}
	}

	static const FixedUnorderedMap<_KeyType, _ValueType, _KeyHashType> Nil;

private:
	size_t _size;
	const _KeyHashType _hasher;

	std::vector<Option<std::pair<_KeyType, _ValueType>>> const _container;
};

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
const FixedUnorderedMap<_KeyType, _ValueType, _KeyHashType> FixedUnorderedMap<_KeyType, _ValueType, _KeyHashType>::Nil(1);