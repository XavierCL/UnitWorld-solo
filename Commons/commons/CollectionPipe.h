#pragma once

#include "Stream.h"

#include <unordered_set>
#include <functional>

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
            return std::make_shared<Stream<OutputValue>>([this, isFirstIter, inputBegin, inputEnd, input]() mutable {
                if (isFirstIter)
                {
                    inputBegin = input->begin();
                    isFirstIter = false;
                }

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
            if (!(inputBegin != inputEnd)) return std::make_shared<Stream<OutputValue>>(EmptyStreamGenerator<OutputValue>());
            auto currentMapping = _mapping(*inputBegin);
            auto innerBegin = currentMapping->begin();
            auto innerEnd = currentMapping->end();
            return std::make_shared<Stream<OutputValue>>([this, inputBegin, inputEnd, input, innerBegin, innerEnd, currentMapping]() mutable {
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
                    const auto returnedValue = OptionS::Some(*innerBegin);
                    ++innerBegin;
                    return returnedValue;
                }
                else
                {
                    return OptionS::None<OutputValue>();
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
            return std::make_shared<Stream<InputValue>>([this, isFirstIter, inputBegin, inputEnd, input]() mutable {
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
            for (const auto inputValue : *input)
            {
                _action(inputValue);
            }
        }

    private:
        const Action _action;
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
                return OptionS::Some(input->at(_value));
            }
            catch (...)
            {
                return OptionS::None<Found>();
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
            auto generatedVector(std::make_shared<std::vector<Value>>());
            generatedVector->insert(generatedVector->begin(), input->begin(), input->end());
            return generatedVector;
        }
    };

    template <typename Key, typename Value, typename KeyHash, typename KeyEqual, typename KeySelector>
    struct ToUnorderedMap
    {
        ToUnorderedMap(const KeySelector& keySelector) :
            _keySelector(keySelector)
        {}

        using Output = std::shared_ptr<std::unordered_map<Key, Value, KeyHash, KeyEqual>>;

        template<typename InputCollection>
        Output operator()(InputCollection input) const
        {
            auto generatedMap(std::make_shared<std::unordered_map<Key, Value, KeyHash, KeyEqual>>());
            for (const auto value : *input)
            {
                (*generatedMap)[_keySelector(value)] = value;
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
            return std::make_shared<Stream<Value>>([input, isFirst, inputBegin, inputEnd]() mutable {
                if (isFirst)
                {
                    inputBegin = input->begin();
                    isFirst = false;
                }

                if (inputBegin != inputEnd)
                {
                    const auto returnedValue(OptionS::Some((*inputBegin).second));
                    ++inputBegin;
                    return returnedValue;
                }
                else
                {
                    return OptionS::None<Value>();
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

FunctionalDefinitions::IsEmpty isEmpty()
{
    return FunctionalDefinitions::IsEmpty();
}

FunctionalDefinitions::Any any()
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
FunctionalDefinitions::ToUnorderedMap<Key, Value, KeyHash, KeyEqual, KeySelector> toUnorderedMap(const KeySelector& keySelector)
{
    return FunctionalDefinitions::ToUnorderedMap<Key, Value, KeyHash, KeyEqual, KeySelector>(keySelector);
}

template <typename Value>
MapExtensions::MapValues<Value> mapValues()
{
    return MapExtensions::MapValues<Value>();
}