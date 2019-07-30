#include "MessageWrapper.h"

#include "CompleteGameStateMessage.h"
#include "InvalidMessage.h"

#include "MessageType.h"

#include <nlohmann/json.hpp>

#include <chrono>

namespace uw
{
    NLOHMANN_JSON_SERIALIZE_ENUM(MessageType, {
        {InvalidMessageType, nullptr},
        {CompleteGameStateMessageType, "complete-game-state"},
    });

    const std::string MessageWrapper::MESSAGE_TYPE_JSON_ATTRIBUTE = "type";
    const std::string MessageWrapper::MESSAGE_DATA_JSON_ATTRIBUTE = "data";
    const std::string MessageWrapper::MESSAGE_TIMESTAMP_JSON_ATTRIBUTE = "timestamp";
}

using namespace uw;

MessageWrapper::MessageWrapper(const std::string& json) :
    _timestamp(jsonToTimestamp(json)),
    _innerMessage(jsonToMessage(json)),
    _json(json)
{}

MessageWrapper::MessageWrapper(const std::shared_ptr<const Message> message) :
    _timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch()).count()),
    _innerMessage(message),
    _json(wrapMessageToJson(message, _timestamp))
{}

std::shared_ptr<Message const> MessageWrapper::innerMessage() const
{
    return _innerMessage;
}

std::string MessageWrapper::json() const
{
    return _json;
}

unsigned long long MessageWrapper::timestamp() const
{
    return _timestamp;
}

std::string MessageWrapper::wrapMessageToJson(const std::shared_ptr<const Message> message, const unsigned long long& timestamp)
{
    nlohmann::json jsonMessage = {
        {MESSAGE_TYPE_JSON_ATTRIBUTE, message->messageType()},
        {MESSAGE_DATA_JSON_ATTRIBUTE, nlohmann::json::parse(message->toJsonData())},
        {MESSAGE_TIMESTAMP_JSON_ATTRIBUTE, timestamp}
    };

    return jsonMessage.dump();
}

std::shared_ptr<const Message> MessageWrapper::jsonToMessage(const std::string& json)
{
    nlohmann::json parsedJson = nlohmann::json::parse(json);
    const auto messageType = parsedJson.at(MESSAGE_TYPE_JSON_ATTRIBUTE).get<MessageType>();
    const auto messageData = parsedJson.at(MESSAGE_DATA_JSON_ATTRIBUTE).dump();

    if (messageType == MessageType::CompleteGameStateMessageType)
    {
        return std::make_shared<const CompleteGameStateMessage>(messageData);
    }
    else
    {
        // Using jsonMessage here because the type was not valid, and the invalid message will display the whole message then
        return std::make_shared<const InvalidMessage>(parsedJson);
    }
}

unsigned long long MessageWrapper::jsonToTimestamp(const std::string& json)
{
    nlohmann::json parsedJson = nlohmann::json::parse(json);

    return parsedJson.at(MESSAGE_TIMESTAMP_JSON_ATTRIBUTE).get<unsigned long long>();
}