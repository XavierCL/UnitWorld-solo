#pragma once

#include "MessageType.h"

#include "nlohmann/json.hpp"

#include <string>

namespace uw
{
    class Message
    {
    public:
        virtual MessageType messageType() const = 0;
        virtual nlohmann::json toJsonData() const = 0;
    };
}