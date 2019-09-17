#pragma once

#include "Option.hpp"

#include <memory>
#include <functional>

template <typename Value>
struct StreamIterator
{
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::input_iterator_tag iterator_category;

    StreamIterator(Option<Value>&& currentValue, std::function<Option<Value>()>&& streamGenerator) :
        _currentValue(std::forward<Option<Value>>(currentValue)),
        _streamGenerator(std::forward<std::function<Option<Value>()>>(streamGenerator))
    {}

    StreamIterator() :
        _currentValue(Options::None<Value>()),
        _streamGenerator([] { return Options::None<Value>(); })
    {}

    StreamIterator(const StreamIterator<Value>& copy) :
        _currentValue(copy._currentValue),
        _streamGenerator(copy._streamGenerator)
    {}

    StreamIterator(StreamIterator<Value>&& moved) :
        _currentValue(std::move(moved._currentValue)),
        _streamGenerator(std::move(moved._streamGenerator))
    {}

    StreamIterator<Value>& operator=(const StreamIterator<Value>& other)
    {
        _currentValue = other._currentValue;
        _streamGenerator = other._streamGenerator;
        return *this;
    }

    StreamIterator<Value>& operator=(StreamIterator<Value>&& moved)
    {
        if (this != &moved)
        {
            _currentValue = std::move(moved._currentValue);
            _streamGenerator = std::move(moved._streamGenerator);
        }
        return *this;
    }

    Value operator*() const
    {
        return _currentValue.getOrThrow();
    }

    Value* operator->() const
    {
        return &_currentValue.getOrThrow();
    }

    StreamIterator<Value>& operator++()
    {
        if (_currentValue.isDefined())
        {
            _currentValue = _streamGenerator();
        }

        return *this;
    }

    StreamIterator<Value>& operator++(int)
    {
        StreamIterator<Value> copy(*this);
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
    StreamIterator s1Copy(std::move(s1));
    s1 = std::move(s2);
    s2 = std::move(s1Copy);
}

template <typename Value>
struct Stream
{
    Stream(const Stream& copy) = delete;

    Stream(Stream&& moved):
        _streamGenerator(std::move(moved._streamGenerator))
    {}

    Stream(const std::function<Option<Value>()>& streamGenerator) = delete;

    Stream(std::function<Option<Value>()>&& streamGenerator) :
        _streamGenerator(std::forward<std::function<Option<Value>()>>(streamGenerator))
    {}

    Stream& operator=(const Stream& copy)
    {
        if (this != &copy)
        {
            _streamGenerator = copy._streamGenerator;
        }
        return *this;
    }

    Stream& operator=(Stream&& moved)
    {
        if (this != &moved)
        {
            _streamGenerator = std::move(moved._streamGenerator);
        }
        return *this;
    }

    StreamIterator<Value> begin()
    {
        Option<Value> firstValue(_streamGenerator());
        return StreamIterator<Value>(std::move(firstValue), std::move(_streamGenerator));
    }

    StreamIterator<Value> end() const
    {
        return StreamIterator<Value>();
    }

private:
    std::function<Option<Value>()> _streamGenerator;
};

struct Streams
{
    template<typename Value>
    static std::shared_ptr<Stream<Value>> generate(std::function<Option<Value>()>&& streamGenerator)
    {
        return std::make_shared<Stream<Value>>(std::forward<std::function<Option<Value>()>>(streamGenerator));
    }

    template<typename Value>
    static std::shared_ptr<Stream<Value>> empty()
    {
        return std::make_shared<Stream<Value>>([] { return Options::None<Value>(); });
    }
};