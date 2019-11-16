#include "MessageWrapper.h"

#include "messages/CompleteGameStateMessage.h"
#include "messages/InvalidMessage.h"
#include "messages/MessageType.h"

#include "messages/commands/MoveMobileUnitsToPositionMessage.h"
#include "messages/commands/MoveMobileUnitsToSpawnerMessage.h"
#include "messages/commands/SetSpawnersRallyMessage.h"

namespace uw
{
    NLOHMANN_JSON_SERIALIZE_ENUM(MessageType, {
        {MessageType::InvalidMessageType, nullptr},
        {MessageType::CompleteGameStateMessageType, "complete-game-state"},
        {MessageType::MoveMobileUnitsToPositionMessageType, "move-units-to-position"},
        {MessageType::MoveMobileUnitsToSpawnerMessageType, "move-units-to-spawner"},
        {MessageType::SetSpawnersRallyMessageType, "set-spawners-rally"}
    });

    const std::string MessageWrapper::MESSAGE_TYPE_JSON_ATTRIBUTE = "type";
    const std::string MessageWrapper::MESSAGE_DATA_JSON_ATTRIBUTE = "data";
    const std::string MessageWrapper::MESSAGE_TIMESTAMP_JSON_ATTRIBUTE = "timestamp";
}

using namespace uw;

std::shared_ptr<Message const> MessageWrapper::innerMessage() const
{
    return _innerMessage;
}

std::string MessageWrapper::json() const
{
    return _json;
}

long long MessageWrapper::timestamp() const
{
    return _timestamp;
}

MessageType MessageWrapper::messageType() const
{
    return _innerMessage->messageType();
}

std::string MessageWrapper::wrapMessageToJson(const std::shared_ptr<const Message> message, const long long& timestamp)
{
    nlohmann::json jsonMessage = {
        {MESSAGE_TYPE_JSON_ATTRIBUTE, message->messageType()},
        {MESSAGE_DATA_JSON_ATTRIBUTE, message->toJsonData()},
        {MESSAGE_TIMESTAMP_JSON_ATTRIBUTE, timestamp}
    };

    return jsonMessage.dump();
}

std::shared_ptr<const Message> MessageWrapper::jsonToMessage(const nlohmann::json& json)
{
    const auto messageType = json.at(MESSAGE_TYPE_JSON_ATTRIBUTE).get<MessageType>();
    const auto messageData = json.at(MESSAGE_DATA_JSON_ATTRIBUTE);

    if (messageType == MessageType::CompleteGameStateMessageType)
    {
        return CompleteGameStateMessage::fromJson(messageData);
    }
    else if (messageType == MessageType::MoveMobileUnitsToPositionMessageType)
    {
        return MoveMobileUnitsToPositionMessage::fromJson(messageData);
    }
    else if (messageType == MessageType::MoveMobileUnitsToSpawnerMessageType)
    {
        return MoveMobileUnitsToSpawnerMessage::fromJson(messageData);
    }
    else if (messageType == MessageType::SetSpawnersRallyMessageType)
    {
        return SetSpawnersRallyMessage::fromJson(messageData);
    }
    else
    {
        // Using jsonMessage here because the type was not valid, and the invalid message will display the whole message then
        return std::make_shared<const InvalidMessage>(json);
    }
}

long long MessageWrapper::jsonToTimestamp(const nlohmann::json& json)
{
    return json.at(MESSAGE_TIMESTAMP_JSON_ATTRIBUTE).get<long long>();
}