#pragma once

#include "Option.hpp"

#include <memory>
#include <functional>

template <typename Value>
struct EmptyStreamGenerator
{
    Option<Value> operator()() const
    {
        return OptionS::None<Value>();
    }
};

template <typename Value>
struct StreamIterator
{
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::input_iterator_tag iterator_category;

    StreamIterator(const Option<Value>& currentValue, const std::function<Option<Value>()> streamGenerator) :
        _currentValue(currentValue),
        _streamGenerator(streamGenerator)
    {}

    StreamIterator(const StreamIterator& copy) :
        _currentValue(copy._currentValue),
        _streamGenerator(copy._streamGenerator)
    {}

    StreamIterator(StreamIterator&& moved) :
        _currentValue(std::move(moved._currentValue)),
        _streamGenerator(std::move(moved._streamGenerator))
    {}

    StreamIterator& operator=(const StreamIterator& other)
    {
        _currentValue = other._currentValue;
        _streamGenerator = other._streamGenerator;
        return *this;
    }

    StreamIterator& operator=(StreamIterator&& moved)
    {
        _currentValue = std::move(moved._currentValue);
        _streamGenerator = std::move(moved._streamGenerator);
    }

    Value operator*() const
    {
        return _currentValue.getOrThrow();
    }

    Value* operator->() const
    {
        return &_currentValue.getOrThrow();
    }

    StreamIterator& operator++()
    {
        if (_currentValue.isDefined())
        {
            _currentValue = _streamGenerator();
        }

        return *this;
    }

    StreamIterator& operator++(int)
    {
        StreamIterator copy(*this);
        ++(*this);
        return copy;
    }

    bool operator!=(const StreamIterator<Value>& other)
    {
        return !_currentValue.isEmpty() || !other._currentValue.isEmpty();
    }

    bool operator==(const StreamIterator<Value>& other)
    {
        return _currentValue.isEmpty() && other._currentValue.isEmpty();
    }

private:
    Option<Value> _currentValue;
    std::function<Option<Value>()> _streamGenerator;
};

template<typename Value>
void swap(StreamIterator<Value>& s1, StreamIterator<Value>& s2)
{
    StreamIterator s1Copy(s1);
    s1 = s2;
    s2 = s1;
}

template <typename Value>
struct Stream {

    Stream(const std::function<Option<Value>()>& streamGenerator) :
        _streamGenerator(streamGenerator)
    {}

    StreamIterator<Value> begin() const
    {
        return StreamIterator<Value>(_streamGenerator(), _streamGenerator);
    }

    StreamIterator<Value> end() const
    {
        return StreamIterator<Value>(OptionS::None<Value>(), EmptyStreamGenerator<Value>());
    }

private:
    const std::function<Option<Value>()> _streamGenerator;
};