#ifndef OPTION_H
#define OPTION_H

#include <exception>

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

    Option(_Type&& inner)
        : _inner(new _Type(inner))
    {}

    Option(const _Type* inner)
        : _inner(inner ? new _Type(*inner) : nullptr)
    {}

    Option(_Type*&& inner)
        : _inner(inner)
    {}

    Option(const Option<_Type>& copy)
        : _inner(newFrom(copy))
    {}

    Option(Option<_Type>&& moved)
        : _inner(std::move(moved._inner))
    {
        moved._inner = nullptr;
    }

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

    Option<_Type>& operator=(Option<_Type>&& moved)
    {
        if (&moved != this)
        {
            remove();
            _inner = moved._inner;
            moved._inner = nullptr;
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

    const _Type& getOrThrow() const
    {
        if (isDefined())
        {
            return *_inner;
        }
        else
        {
            throw std::logic_error("Tried to access an empty option");
        }
    }

    _Type getOrElse(const _Type& defaultValue) const
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
    _Type getOrElse(const _FunctionType& defaultGenerator) const
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

    Option<_Type> orElse(Option<_Type>&& defaultOption) const
    {
        if (isDefined())
        {
            return *this;
        }
        else
        {
            return std::forward<Option<_Type>>(defaultOption);
        }
    }

    template <typename _FunctionType>
    Option<_Type> orElse(const _FunctionType& defaultOptionGenerator) const
    {
        if (isDefined())
        {
            return *this;
        }
        else
        {
            return defaultOptionGenerator();
        }
    }

    template <typename _FunctionType>
    const Option<_Type>& orExecute(_FunctionType&& elseAction) const
    {
        if (isEmpty())
        {
            elseAction();
        }
        return *this;
    }

    template <typename _ReturnType, typename _FunctionType>
    Option<_ReturnType> map(const _FunctionType& mappingFunction) const
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
    const Option<_Type>& foreach(const _FunctionType& foreachFunction) const
    {
        if (isDefined())
        {
            foreachFunction(*_inner);
        }
        return *this;
    }

    template <typename _PredicateType>
    Option<_Type> filter(const _PredicateType& predicate) const
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

struct Options
{
    template <typename Type>
    static Option<Type> Some(const Type& inner)
    {
        return Option<Type>(inner);
    }

    template<typename Type>
    static Option<Type> None()
    {
        return Option<Type>();
    }
};

#endif