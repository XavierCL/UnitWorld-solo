#pragma once

#include "Message.h"

namespace uw
{
    class InvalidMessage : public Message
    {
    public:
        InvalidMessage(const nlohmann::json& invalidData):
            _invalidJsonData(invalidData)
        {}

        virtual MessageType messageType() const
        {
            return MessageType::InvalidMessageType;
        }

        virtual nlohmann::json toJsonData() const
        {
            return _invalidJsonData;
        }

    private:
        const nlohmann::json _invalidJsonData;
    };
}