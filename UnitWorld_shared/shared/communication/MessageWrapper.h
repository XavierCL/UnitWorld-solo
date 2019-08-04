#pragma once

#include "messages/Message.h"

namespace uw
{
    class MessageWrapper
    {
    public:
        MessageWrapper(const std::string& json);

        MessageWrapper(const std::shared_ptr<const Message> message);

        std::shared_ptr<Message const> innerMessage() const;

        std::string json() const;

        unsigned long long timestamp() const;

        MessageType messageType() const;

    private:

        static const std::string MESSAGE_TYPE_JSON_ATTRIBUTE;
        static const std::string MESSAGE_DATA_JSON_ATTRIBUTE;
        static const std::string MESSAGE_TIMESTAMP_JSON_ATTRIBUTE;

        static std::string wrapMessageToJson(const std::shared_ptr<const Message> message, const unsigned long long& timestamp);
        static std::shared_ptr<const Message> jsonToMessage(const std::string& json);
        static unsigned long long jsonToTimestamp(const std::string& json);

        const unsigned long long _timestamp;
        const std::shared_ptr<const Message> _innerMessage;
        const std::string _json;
    };
}