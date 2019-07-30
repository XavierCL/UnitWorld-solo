#pragma once

#include "MessageWrapper.h"

#include "commons/StringsHelper.h"

namespace uw
{
    class MessageSerializer
    {
    public:
        std::string serialize(std::vector<std::shared_ptr<const MessageWrapper>> messages) const
        {
            std::string communication;

            for (const auto message : messages)
            {
                communication += message->json() + END_MESSAGE_FLAG;
            }

            return communication;
        }

        std::vector<MessageWrapper> deserialize(std::string currentCommunication)
        {
            auto wholeCommunication = _remainingDeserializationBuffer + currentCommunication;
            auto communications(StringsHelper::split(wholeCommunication, END_MESSAGE_FLAG));

            if (communications.back().empty())
            {
                _remainingDeserializationBuffer.clear();
                communications.erase(communications.end());
            }
            else
            {
                _remainingDeserializationBuffer = communications.back();
                communications.erase(communications.end());
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