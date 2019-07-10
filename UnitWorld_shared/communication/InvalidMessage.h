#pragma once

#include "Message.h"

namespace uw
{
    class InvalidMessage : public Message
    {
    public:
        InvalidMessage(const std::string& invalidData):
            _invalidJsonData(invalidData)
        {}

        virtual MessageType messageType() const
        {
            return MessageType::InvalidMessageType;
        }

        virtual std::string toJsonData() const
        {
            throw new std::exception(("Cannot convert an invalid message to json. The message was: " + _invalidJsonData).c_str());
        }

    private:
        const std::string _invalidJsonData;
    };
}