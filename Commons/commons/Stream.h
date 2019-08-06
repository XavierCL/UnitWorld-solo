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
    StreamIterator(const Option<Value>& currentValue, const std::function<Option<Value>()> streamGenerator) :
        _currentValue(currentValue),
        _streamGenerator(streamGenerator)
    {}

    Value operator*() const
    {
        return _currentValue.getOrThrow();
    }

    StreamIterator& operator++()
    {
        if (_currentValue.isDefined())
        {
            _currentValue = _streamGenerator();
        }

        return *this;
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