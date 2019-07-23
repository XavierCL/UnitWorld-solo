#include "MessageWrapper.h"

#include "CompleteGameStateMessage.h"
#include "InvalidMessage.h"

#include "MessageType.h"

#include <nlohmann/json.hpp>

namespace uw
{
    NLOHMANN_JSON_SERIALIZE_ENUM(MessageType, {
        {InvalidMessageType, nullptr},
        {CompleteGameStateMessageType, "complete-game-state"},
    });

    const std::string MessageWrapper::MESSAGE_TYPE_JSON_ATTRIBUTE = "type";
    const std::string MessageWrapper::MESSAGE_DATA_JSON_ATTRIBUTE = "data";
}

using namespace uw;

MessageWrapper::MessageWrapper(const std::string& json) :
    _json(json),
    _innerMessage(stringToMessage(json))
{}

MessageWrapper::MessageWrapper(const std::shared_ptr<const Message> message) :
    _json(wrapMessageToJson(message)),
    _innerMessage(message)
{}

std::shared_ptr<Message const> MessageWrapper::innerMessage() const
{
    return _innerMessage;
}

std::string MessageWrapper::json() const
{
    return _json;
}

std::string MessageWrapper::wrapMessageToJson(const std::shared_ptr<const Message> message)
{
    nlohmann::json jsonMessage = {
        {MESSAGE_TYPE_JSON_ATTRIBUTE, message->messageType()},
        {MESSAGE_DATA_JSON_ATTRIBUTE, message->toJsonData()}
    };
    return jsonMessage;
}

std::shared_ptr<const Message> MessageWrapper::stringToMessage(const std::string& json)
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