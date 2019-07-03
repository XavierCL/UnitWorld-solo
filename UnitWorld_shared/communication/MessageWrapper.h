#pragma once

#include "CompleteGameStateMessage.h"
#include "InvalidMessage.h"

#include "Message.h"
#include "MessageType.h"

#include <nlohmann/json.hpp>

namespace uw
{
    class MessageWrapper
    {
    public:
        MessageWrapper(const std::string& json):
            _json(json),
            _innerMessage(stringToMessage(json))
        {}

        MessageWrapper(const std::shared_ptr<const Message> message):
            _json(wrapMessageToJson(message)),
            _innerMessage(message)
        {}

        std::shared_ptr<const Message> innerMessage() const
        {
            return _innerMessage;
        }

        std::string json() const
        {
            return _json;
        }

    private:

        static const std::string MESSAGE_TYPE_JSON_ATTRIBUTE;
        static const std::string MESSAGE_DATA_JSON_ATTRIBUTE;

        static std::string wrapMessageToJson(const std::shared_ptr<const Message> message)
        {
            nlohmann::json jsonMessage = {
                {MESSAGE_TYPE_JSON_ATTRIBUTE, message->messageType()},
                {MESSAGE_DATA_JSON_ATTRIBUTE, message->toJsonData()}
            };
            return jsonMessage;
        }

        static std::shared_ptr<const Message> stringToMessage(const std::string& json)
        {
            nlohmann::json jsonMessage = nlohmann::json::parse(json);
            const auto messageType = jsonMessage.at(MESSAGE_TYPE_JSON_ATTRIBUTE).get<MessageType>();
            const auto messageData = jsonMessage.at(MESSAGE_DATA_JSON_ATTRIBUTE).get<std::string>();

            if (messageType == MessageType::CompleteGameStateMessageType)
            {
                return std::make_shared<const CompleteGameStateMessage>(messageData);
            }
            else
            {
                // Using jsonMessage here because the type was not valid, and the invalid message will display the whole message then
                return std::make_shared<const InvalidMessage>(jsonMessage);
            }
        }

        const std::string _json;
        const std::shared_ptr<const Message> _innerMessage;
    };
}