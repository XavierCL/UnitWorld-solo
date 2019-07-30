#pragma once

#include "MessageWrapper.h"

#include "commons/StringsHelper.h"

namespace uw
{
    class MessageSerializer
    {
    public:
        std::string serialize(std::vector<MessageWrapper> messages) const
        {
            std::string communication;

            for (const auto message : messages)
            {
                communication += message.json() + END_MESSAGE_FLAG;
            }

            return communication;
        }

        std::vector<MessageWrapper> deserialize(std::string currentCommunication)
        {
            auto wholeCommunication = _remainingDeserializationBuffer + currentCommunication;
            auto communications(StringsHelper::split(wholeCommunication, END_MESSAGE_FLAG));

            if (communications.back().front() == '\0')
            {
                _remainingDeserializationBuffer.clear();
                communications.pop_back();
            }
            else
            {
                _remainingDeserializationBuffer = communications.back();
                communications.pop_back();
            }

            std::vector<MessageWrapper> messages;
            for (const auto communication : communications)
            {
                messages.emplace_back(MessageWrapper(communication));
            }

            return messages;
        }

    private:

        const std::string END_MESSAGE_FLAG = "\\n";

        std::string _remainingDeserializationBuffer;
    };
}