#pragma once

#include "Stream.h"

#include <unordered_set>
#include <functional>

template <typename InputCollection, typename StreamModifier, typename Output = typename StreamModifier::Output>
Output operator|(const InputCollection& input, const StreamModifier& modifier)
{
    return modifier(input);
}

struct Any
{
    using Output = bool;

    template <typename InputCollection>
    bool operator()(const InputCollection& input) const
    {
        return input.begin() != input.end();
    }
};

struct IsEmpty
{
    using Output = bool;

    template <typename InputCollection>
    bool operator() (const InputCollection& input) const
    {
        return input.begin() == input.end();
    }
};

template <typename InputValue, typename OutputValue>
struct Map
{
    Map(const std::function<OutputValue(InputValue)>& mapping) :
        _mapping(mapping)
    {}

    using Output = Stream<OutputValue>;

    template<typename InputCollection>
    Output operator()(const InputCollection& input) const
    {
        auto inputBegin = input.begin();
        auto inputEnd = input.end();
        return Stream<OutputValue>([this, inputBegin, inputEnd] () mutable {
            if (inputBegin != inputEnd)
            {
                const auto returnedValue = OptionS::Some(_mapping(*inputBegin));
                ++inputBegin;
                return returnedValue;
            }
            else
            {
                return OptionS::None<OutputValue>();
            }
        });
    }
private:
    const std::function<OutputValue(InputValue)> _mapping;
};

template <typename InputValue>
struct Filter
{
    Filter(const std::function<bool(InputValue)>& filter) :
        _filter(filter)
    {}

    using Output = Stream<InputValue>;

    template<typename InputCollection>
    Stream<InputValue> operator()(const InputCollection& input) const
    {
        auto inputBegin = input.begin();
        auto inputEnd = input.end();
        return Stream<InputValue>([this, inputBegin, inputEnd]() mutable {
            while (inputBegin != inputEnd && !_filter(*inputBegin))
            {
                ++inputBegin;
            }
            if (inputBegin != inputEnd)
            {
                const auto returnedValue = OptionS::Some(*inputBegin);
                ++inputBegin;
                return returnedValue;
            }
            else
            {
                return OptionS::None<InputValue>();
            }
        });
    }

private:
    const std::function<bool(InputValue)> _filter;
};

template <typename InputValue>
struct ForEach
{
    ForEach(const std::function<void(InputValue)>& action) :
        _action(action)
    {}

    using Output = void;

    template <typename InputCollection>
    Output operator() (const InputCollection& input) const
    {
        for (const auto inputValue: input)
        {
            _action(inputValue);
        }
    }

private:
    const std::function<void(InputValue)> _action;
};

template <typename Value, typename ValueHash = std::hash<Value>, typename ValueEqual = std::equal_to<Value>>
struct ToUnorderedSet
{
    using Output = std::unordered_set<Value, ValueHash, ValueEqual>;

    template<typename InputCollection>
    Output operator()(const InputCollection& input) const
    {
        return std::unordered_set<Value, ValueHash, ValueEqual>(input.begin(), input.end());
    }
};