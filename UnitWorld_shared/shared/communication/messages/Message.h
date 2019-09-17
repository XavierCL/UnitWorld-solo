#pragma once

#include "MessageType.h"

#include <string>

namespace uw
{
    class Message
    {
    public:
        virtual MessageType messageType() const = 0;
        virtual std::string toJsonData() const = 0;
    };
}