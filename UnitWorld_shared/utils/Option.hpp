#pragma once

template <typename _Type>
class Option
{
public:
	Option<_Type>(const _Type&& copy):
		_inner(new _Type(copy))
	{}

	Option<_Type>()
	{
		_inner(nullptr);
	}

	void isDefined() const
	{
		return _inner != nullptr;
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

	~Option<_Type>()
	{
		delete _inner;
	}

private:
	const _Type* _inner;
};