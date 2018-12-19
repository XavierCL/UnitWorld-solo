#ifndef OPTION_H
#define OPTION_H

template <typename _Type>
class Option
{
public:

	Option()
		: _inner(nullptr)
	{}

	Option(const _Type& inner)
		: _inner(new _Type(inner))
	{}

	Option(const _Type* inner)
		: _inner(inner ? new _Type(*inner) : nullptr)
	{}

	Option(const Option<_Type>& copy)
		: _inner(newFrom(copy))
	{}

	~Option()
	{
		remove();
	}

	Option<_Type>& operator=(const Option<_Type>& other)
	{
		if (&other != this)
		{
			remove();
			_inner = newFrom(other);
		}
		return *this;
	}

	const bool isDefined() const
	{
		return _inner != nullptr;
	}

	virtual const bool isEmpty() const
	{
		return _inner == nullptr;
	}

	const _Type& getOrElse(const _Type& defaultValue) const
	{
		if (isDefined())
		{
			return *_inner;
		}
		else
		{
			return defaultValue;
		}
	}

	template <typename _FunctionType>
	const _Type getOrElse(const _FunctionType& defaultGenerator) const
	{
		if (isDefined())
		{
			return *_inner;
		}
		else
		{
			return defaultGenerator();
		}
	}

	template <typename _ReturnType, typename _FunctionType>
	const Option<_ReturnType> map(const _FunctionType& mappingFunction) const
	{
		if (isDefined())
		{
			return Option<_ReturnType>(mappingFunction(*_inner));
		}
		else
		{
			return Option<_ReturnType>();
		}
	}

	template <typename _ReturnType, typename _FunctionType>
	const Option<_ReturnType> flatMap(const _FunctionType& mappingFunction) const
	{
		if (isDefined())
		{
			return mappingFunction(*_inner);
		}
		else
		{
			return Option<_ReturnType>();
		}
	}

	template <typename _FunctionType>
	void foreach(const _FunctionType& foreachFunction) const
	{
		if (isDefined())
		{
			foreachFunction(*_inner);
		}
	}

	template <typename _PredicateType>
	const Option<_Type> filter(const _PredicateType& predicate) const
	{
		if (isDefined())
		{
			if (predicate(*_inner))
			{
				return *this;
			}
			else
			{
				return Option<_Type>();
			}
		}
		else
		{
			return *this;
		}
	}

	const bool operator==(const Option<_Type>& other) const
	{
		if (isDefined() && other.isDefined())
		{
			return *_inner == *other._inner;
		}
		else if (isEmpty() && other.isEmpty())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	const bool operator!=(const Option<_Type>& other) const
	{
		return !operator==(other);
	}

private:
	void remove()
	{
		if (isDefined())
		{
			delete _inner;
		}
	}

	const _Type* newFrom(const Option<_Type>& other)
	{
		return other.isDefined() ? new _Type(*other._inner) : nullptr;
	}

	const _Type* _inner;
};

#endif