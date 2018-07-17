#pragma once

#include <memory>

template <typename _Type>
class Option
{
public:

	Option()
	{}

	Option(const _Type& inner):
		_inner(std::make_shared<const _Type>(inner))
	{}

	Option(const std::shared_ptr<_Type>& inner):
		_inner(inner)
	{}

	Option(const Option<_Type>& copy):
		_inner(copy._inner)
	{}

	Option(Option<_Type>&& moved):
		_inner(std::move(moved._inner))
	{}

	Option<_Type>& operator=(const Option<_Type>& copy)
	{
		if (this != &copy)
		{
			_inner = copy._inner;
		}
		return *this;
	}

	Option<_Type>& operator=(Option<_Type>&& moved)
	{
		if (this != &moved)
		{
			_inner = std::move(moved._inner);
		}
		return *this;
	}

	bool defined() const
	{
		return !_inner;
	}

	bool empty() const
	{
		return static_cast<bool>(_inner);
	}

	std::shared_ptr<const _Type> getOrElse(const std::shared_ptr<const _Type>& defaultValue) const
	{
		if (isDefined())
		{
			return _inner;
		}
		else
		{
			return defaultValue;
		}
	}

	template <typename _FunctionType>
	std::shared_ptr<const _Type> getOrElse(const _FunctionType& defaultGenerator) const
	{
		if (isDefined())
		{
			return _inner;
		}
		else
		{
			return defaultGenerator();
		}
	}

	template <typename _ReturnType, typename _FunctionType>
	Option<_ReturnType> map(const _FunctionType& mappingFunction) const
	{
		if (isDefined())
		{
			return Option<_ReturnType>(mappingFunction(_inner));
		}
		else
		{
			return Option<_ReturnType>();
		}
	}

	template <typename _ReturnType, typename _FunctionType>
	Option<_ReturnType> flatMap(const _FunctionType& mappingFunction) const
	{
		if (isDefined())
		{
			return mappingFunction(_inner);
		}
		else
		{
			return Option<_ReturnType>();
		}
	}

	template <typename _InnerType>
	Option<_InnerType> flatten() const
	{
		getOrElse(Option<_InnerType>());
	}

	template <typename _FunctionType>
	void foreach(const _FunctionType& foreachFunction) const
	{
		if (isDefined())
		{
			foreachFunction(_inner);
		}
	}

	template <typename _PredicateType>
	Option<_Type> filter(const _PredicateType& predicate) const
	{
		if (isDefined())
		{
			return predicate(*_inner))
				? return *this
				: Option<_Type>();
		}
		else
		{
			return Option<_Type>();
		}
	}

	bool same(const Option<_Type>& other) const
	{
		return _inner == other._inner;
	}

	bool operator==(const Option<_Type>& other) const
	{
		if (isDefined() && other.isDefined())
		{
			return *_inner == *other._inner;
		}
		else
		{
			return isEmpty() && other.isEmpty();
		}
	}

	bool operator!=(const Option<_Type>& other) const
	{
		return !operator==(other);
	}

	static const Option<_Type> None;

private:
	std::shared_ptr<const _Type> _inner;
};

template <_Type>
const Option<_Type> Option<_Type>::None();
