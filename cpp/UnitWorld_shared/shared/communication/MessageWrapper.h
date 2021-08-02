#pragma once

#include "messages/Message.h"

#include <nlohmann/json.hpp>

#include <chrono>

namespace uw
{
    class MessageWrapper
    {
    public:

        MessageWrapper(const std::string& json, const std::shared_ptr<const Message> message, const long long timestamp):
            _json(json),
            _innerMessage(message),
            _timestamp(timestamp)
        {}

        std::shared_ptr<Message const> innerMessage() const;

        std::string json() const;

        long long timestamp() const;

        MessageType messageType() const;

        static MessageWrapper fromJson(const std::string& json)
        {
            nlohmann::json parsedJson = nlohmann::json::parse(json);

            return MessageWrapper(json, jsonToMessage(parsedJson), jsonToTimestamp(parsedJson));
        }

        static MessageWrapper fromMessage(const std::shared_ptr<Message> message)
        {
            const long long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch()).count();

            return MessageWrapper(wrapMessageToJson(message, timestamp), message, timestamp);
        }

    private:

        static const std::string MESSAGE_TYPE_JSON_ATTRIBUTE;
        static const std::string MESSAGE_DATA_JSON_ATTRIBUTE;
        static const std::string MESSAGE_TIMESTAMP_JSON_ATTRIBUTE;

        static std::string wrapMessageToJson(const std::shared_ptr<const Message> message, const long long& timestamp);
        static std::shared_ptr<const Message> jsonToMessage(const nlohmann::json& json);
        static long long jsonToTimestamp(const nlohmann::json& json);

        const long long _timestamp;
        const std::shared_ptr<const Message> _innerMessage;
        const std::string _json;
    };
}