#pragma once

#include "Message.h"

namespace uw
{
    class CompleteGameStateMessage : public Message
    {
    public:
        CompleteGameStateMessage()
        {}

        virtual MessageType messageType() const = 0;
        virtual std::string toJsonData() const = 0;
    };
}