#pragma once

#include "Stream.h"

#include <unordered_set>
#include <functional>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

struct FunctionalDefinitions
{
    struct Any
    {
        using Output = bool;

        template <typename InputCollection>
        bool operator()(InputCollection input) const
        {
            return input->begin() != input->end();
        }
    };

    struct IsEmpty
    {
        using Output = bool;

        template <typename InputCollection>
        bool operator() (InputCollection input) const
        {
            return input.begin() == input->end();
        }
    };

    template <typename OutputValue, typename Mapping>
    struct Map
    {
        Map(const Mapping& mapping) :
            _mapping(mapping)
        {}

        using Output = std::shared_ptr<Stream<OutputValue>>;

        template<typename InputCollection>
        Output operator()(InputCollection input) const
        {
            bool isFirstIter = true;
            auto inputBegin = input->end();
            auto inputEnd = input->end();
            return Streams::generate<OutputValue>([this, isFirstIter, inputBegin, inputEnd, input]() mutable {
                if (isFirstIter)
                {
                    inputBegin = input->begin();
                    isFirstIter = false;
                }

                if (inputBegin != inputEnd)
                {
                    const auto returnedValue = Options::Some(_mapping(*inputBegin));
                    ++inputBegin;
                    return returnedValue;
                }
                else
                {
                    return Options::None<OutputValue>();
                }
            });
        }
    private:
        const Mapping _mapping;
    };

    template <typename OutputValue, typename Mapping>
    struct FlatMap
    {
        FlatMap(const Mapping& mapping) :
            _mapping(mapping)
        {}

        using Output = std::shared_ptr<Stream<OutputValue>>;

        template<typename InputCollection>
        Output operator()(const InputCollection input) const
        {
            auto inputBegin = input->begin();
            const auto inputEnd = input->end();
            if (inputBegin == inputEnd) return Streams::empty<OutputValue>();
            auto currentMapping = _mapping(*inputBegin);
            auto innerBegin = currentMapping->begin();
            auto innerEnd = currentMapping->end();
            return Streams::generate<OutputValue>([this, inputBegin, inputEnd, input, innerBegin, innerEnd, currentMapping]() mutable {
                while (inputBegin != inputEnd && !(innerBegin != innerEnd))
                {
                    ++inputBegin;
                    if (inputBegin != inputEnd)
                    {
                        currentMapping = _mapping(*inputBegin);
                        innerBegin = currentMapping->begin();
                        innerEnd = currentMapping->end();
                    }
                }
                if (innerBegin != innerEnd)
                {
                    const auto returnedValue = Options::Some(*innerBegin);
                    ++innerBegin;
                    return returnedValue;
                }
                else
                {
                    return Options::None<OutputValue>();
                }
            });
        }

    private:
        const Mapping _mapping;
    };

    template <typename InputValue>
    struct Filter
    {
        Filter(const std::function<bool(InputValue)>& filter) :
            _filter(filter)
        {}

        using Output = std::shared_ptr<Stream<InputValue>>;

        template<typename InputCollection>
        Output operator()(InputCollection input) const
        {
            bool isFirstIter = true;
            auto inputBegin = input->end();
            auto inputEnd = input->end();
            return Streams::generate<InputValue>([this, isFirstIter, inputBegin, inputEnd, input]() mutable {
                if (isFirstIter)
                {
                    inputBegin = input->begin();
                    isFirstIter = false;
                }

                while (inputBegin != inputEnd && !_filter(*inputBegin))
                {
                    ++inputBegin;
                }
                if (inputBegin != inputEnd)
                {
                    const auto returnedValue = Options::Some(*inputBegin);
                    ++inputBegin;
                    return returnedValue;
                }
                else
                {
                    return Options::None<InputValue>();
                }
            });
        }

    private:
        const std::function<bool(InputValue)> _filter;
    };

    template <typename Value>
    struct Reverse
    {
        using Output = std::shared_ptr<Stream<Value>>;

        template <typename InputCollection>
        Output operator() (InputCollection input) const
        {
            bool isFirstIter = true;
            auto inputBegin = input->rend();
            auto inputEnd = input->rend();
            return Streams::generate<Value>([isFirstIter, inputBegin, inputEnd, input]() mutable {
                if (isFirstIter)
                {
                    isFirstIter = false;
                    inputBegin = input->rbegin();
                }

                if (inputBegin != inputEnd)
                {
                    const auto returnedValue = Options::Some(*inputBegin);
                    ++inputBegin;
                    return returnedValue;
                }
                else
                {
                    return Options::None<Value>();
                }
            });
        }
    };

    template <typename Action>
    struct ForEach
    {
        ForEach(const Action& action) :
            _action(action)
        {}

        using Output = void;

        template <typename InputCollection>
        void operator() (InputCollection input) const
        {
            auto inputCopy = input;
            for (const auto inputValue : *inputCopy)
            {
                _action(inputValue);
            }
        }

    private:
        const Action _action;
    };

    template<typename Found, typename Predicate>
    struct First
    {
        First(const Predicate& predicate) :
            _predicate(predicate)
        {}

        using Output = Option<Found>;

        template <typename InputCollection>
        Output operator() (InputCollection input) const
        {
            auto inputCopy = input;
            for (auto inputValue : *inputCopy)
            {
                if (_predicate(inputValue))
                {
                    return Options::Some(inputValue);
                }
            }
            return Options::None<Found>();
        }

    private:
        const Predicate _predicate;
    };

    template<typename Found, typename Value>
    struct Find
    {
        Find(const Value& value) :
            _value(value)
        {}

        using Output = Option<Found>;

        template <typename InputCollection>
        Output operator() (InputCollection input) const
        {
            try
            {
                return Options::Some(input->at(_value));
            }
            catch (...)
            {
                return Options::None<Found>();
            }
        }

    private:
        const Value _value;
    };

    template <typename Value, typename ValueHash, typename ValueEqual>
    struct ToUnorderedSet
    {
        using Output = std::shared_ptr<std::unordered_set<Value, ValueHash, ValueEqual>>;

        template<typename InputCollection>
        Output operator()(InputCollection input) const
        {
            return std::make_shared<std::unordered_set<Value, ValueHash, ValueEqual>>(input->begin(), input->end());
        }
    };

    template <typename Value>
    struct ToVector
    {
        using Output = std::shared_ptr<std::vector<Value>>;

        template<typename InputCollection>
        Output operator()(InputCollection input) const
        {
            return std::make_shared<std::vector<Value>>(input->begin(), input->end());
        }
    };

    template <typename Key, typename Value, typename KeyHash, typename KeyEqual, typename KeySelector, typename ValueSelector>
    struct ToUnorderedMap
    {
        ToUnorderedMap(const KeySelector& keySelector, const ValueSelector& valueSelector) :
            _keySelector(keySelector),
            _valueSelector(valueSelector)
        {}

        using Output = std::shared_ptr<std::unordered_map<Key, Value, KeyHash, KeyEqual>>;

        template<typename InputCollection>
        Output operator()(InputCollection input) const
        {
            auto generatedMap(std::make_shared<std::unordered_map<Key, Value, KeyHash, KeyEqual>>());
            auto inputCopy = input;
            for (const auto value : *inputCopy)
            {
                (*generatedMap)[_keySelector(value)] = _valueSelector(value);
            }
            return generatedMap;
        }

    private:
        const KeySelector _keySelector;
        const ValueSelector _valueSelector;
    };

    template <typename Key, typename Value, typename KeyHash, typename KeyEqual, typename KeySelector>
    struct GroupBy
    {
        GroupBy(const KeySelector& keySelector) :
            _keySelector(keySelector)
        {}

        using Output = std::shared_ptr<std::unordered_map<Key, std::vector<Value>, KeyHash, KeyEqual>>;

        template <typename InputCollection>
        Output operator()(InputCollection input) const
        {
            auto generatedMap(std::make_shared<std::unordered_map<Key, std::vector<Value>, KeyHash, KeyEqual>>());
            auto inputCopy = input;
            for (const auto& value : *inputCopy)
            {
                (*generatedMap)[_keySelector(value)].push_back(value);
            }
            return generatedMap;
        }

    private:
        const KeySelector _keySelector;
    };
};

struct MapExtensions
{
    template <typename Value>
    struct MapValues
    {
        using Output = std::shared_ptr<Stream<Value>>;

        template <typename InputCollection>
        Output operator() (InputCollection input) const
        {
            bool isFirst(true);
            auto inputBegin(input->end());
            auto inputEnd(input->end());
            return Streams::generate<Value>([input, isFirst, inputBegin, inputEnd]() mutable {
                if (isFirst)
                {
                    inputBegin = input->begin();
                    isFirst = false;
                }

                if (inputBegin != inputEnd)
                {
                    const auto returnedValue(Options::Some((*inputBegin).second));
                    ++inputBegin;
                    return returnedValue;
                }
                else
                {
                    return Options::None<Value>();
                }
            });
        }
    };
};

template <typename PointerToCollection, typename StreamModifier, typename Output = typename StreamModifier::Output>
Output operator|(PointerToCollection input, const StreamModifier& modifier)
{
    return modifier(input);
}

inline FunctionalDefinitions::IsEmpty isEmpty()
{
    return FunctionalDefinitions::IsEmpty();
}

inline FunctionalDefinitions::Any any()
{
    return FunctionalDefinitions::Any();
}

template <typename OutputValue, typename Mapping>
FunctionalDefinitions::Map<OutputValue, Mapping> map(const Mapping& mapping)
{
    return FunctionalDefinitions::Map<OutputValue, Mapping>(mapping);
}

template <typename OutputValue, typename Mapping>
FunctionalDefinitions::FlatMap<OutputValue, Mapping> flatMap(const Mapping& mapping)
{
    return FunctionalDefinitions::FlatMap<OutputValue, Mapping>(mapping);
}

template <typename InputValue>
FunctionalDefinitions::Filter<InputValue> filter(const std::function<bool(InputValue)>& filterFunction)
{
    return FunctionalDefinitions::Filter<InputValue>(filterFunction);
}

template <typename Action>
FunctionalDefinitions::ForEach<Action> forEach(const Action& action)
{
    return FunctionalDefinitions::ForEach<Action>(action);
}

template <typename Value>
FunctionalDefinitions::Reverse<Value> reverse()
{
    return FunctionalDefinitions::Reverse<Value>();
}

template <typename Found, typename Predicate>
FunctionalDefinitions::First<Found, Predicate> first(const Predicate& predicate)
{
    return FunctionalDefinitions::First<Found, Predicate>(predicate);
}

template <typename Found, typename Value>
FunctionalDefinitions::Find<Found, Value> find(const Value& value)
{
    return FunctionalDefinitions::Find<Found, Value>(value);
}

template <typename Value, typename ValueHash = std::hash<Value>, typename ValueEqual = std::equal_to<Value>>
FunctionalDefinitions::ToUnorderedSet<Value, ValueHash, ValueEqual> toUnorderedSet()
{
    return FunctionalDefinitions::ToUnorderedSet<Value, ValueHash, ValueEqual>();
}

template <typename Value>
FunctionalDefinitions::ToVector<Value> toVector()
{
    return FunctionalDefinitions::ToVector<Value>();
}

template <typename Key, typename Value, typename KeyHash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>, typename KeySelector>
FunctionalDefinitions::ToUnorderedMap<Key, Value, KeyHash, KeyEqual, KeySelector, std::function<Value(Value)>> toUnorderedMap(const KeySelector& keySelector)
{
    return FunctionalDefinitions::ToUnorderedMap<Key, Value, KeyHash, KeyEqual, KeySelector, std::function<Value(Value)>>(keySelector, [](const auto& value) { return value;  });
}

template <typename Key, typename Value, typename KeyHash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>, typename KeySelector, typename ValueSelector>
FunctionalDefinitions::ToUnorderedMap<Key, Value, KeyHash, KeyEqual, KeySelector, ValueSelector> toUnorderedMap(const KeySelector& keySelector, const ValueSelector& valueSelector)
{
    return FunctionalDefinitions::ToUnorderedMap<Key, Value, KeyHash, KeyEqual, KeySelector, ValueSelector>(keySelector, valueSelector);
}

template <typename Key, typename Value, typename KeyHash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>, typename KeySelector>
FunctionalDefinitions::GroupBy<Key, Value, KeyHash, KeyEqual, KeySelector> groupBy(const KeySelector& keySelector)
{
    return FunctionalDefinitions::GroupBy<Key, Value, KeyHash, KeyEqual, KeySelector>(keySelector);
}

template <typename Value>
MapExtensions::MapValues<Value> mapValues()
{
    return MapExtensions::MapValues<Value>();
}