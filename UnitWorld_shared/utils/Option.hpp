#pragma once

template <typename _Type>
class Option
{
public:
	Option<_Type>(const _Type& copy):
		_inner(new _Type(copy))
	{}

	Option<_Type>(const _Type const* &copy)
	{
		if(copy)
		{
			_inner(new _Type(*copy));
		}
		else
		{
			_inner = nullptr;
		}
	}

	Option<_Type>():
		_inner(nullptr)
	{}

	const bool isDefined() const
	{
		return _inner != nullptr;
	}

	const bool isEmpty() const
	{
		return _inner == nullptr;
	}

	_Type& getOrElse(const _Type&& default) const
	{
		if(_inner)
		{
			return *_inner
		}
		else
		{
			return default;
		}
	}

	template <typename _ReturnType, typename _FunctionType>
	Option<_ReturnType> map(const _FunctionType&& mappingFunction) const
	{
		if(_inner)
		{
			return Option<_ReturnType>(mappingFunction(*_inner));
		}
		else
		{
			return Option<_ReturnType>();
		}
	}

	template <typename _ReturnType, typename _Function>
	Option<_ReturnType> flatMap(const _FunctionType&& mappingFunction) const
	{
		if(_inner)
		{
			return mappingFunction(*_inner);
		}
		else
		{
			return Option<_ReturnType>();
		}
	}

	template <typename _FunctionType>
	void foreach(const _FunctionType&& foreachFunction) const
	{
		if(_inner)
		{
			foreachFunction(*_inner);
		}
	}

	~Option<_Type>()
	{
		delete _inner;
	}

	static const Option<_Type> none;

private:
	const _Type* _inner;
};