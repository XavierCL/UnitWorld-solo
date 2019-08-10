#pragma once

#include <memory>

template <typename Type>
std::shared_ptr<Type> make_shared(Type&& value)
{
    return std::make_shared<Type>(value);
}