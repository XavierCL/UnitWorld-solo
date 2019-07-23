#pragma once

#include "Message.h"

namespace uw
{
    class MessageWrapper
    {
    public:
        MessageWrapper(const std::string& json);

        MessageWrapper(const std::shared_ptr<const Message> message);

        std::shared_ptr<Message const> innerMessage() const;

        std::string json() const;

    private:

        static const std::string MESSAGE_TYPE_JSON_ATTRIBUTE;
        static const std::string MESSAGE_DATA_JSON_ATTRIBUTE;

        static std::string wrapMessageToJson(const std::shared_ptr<const Message> message);

        static std::shared_ptr<const Message> stringToMessage(const std::string& json);

        const std::string _json;
        const std::shared_ptr<const Message> _innerMessage;
    };
}