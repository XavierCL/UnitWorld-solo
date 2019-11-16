#pragma once

#include "MessageWrapper.h"

#include "commons/StringsHelper.h"
#include "commons/Logger.hpp"

namespace uw
{
    class MessageSerializer
    {
    public:
        std::string serialize(std::vector<std::shared_ptr<MessageWrapper>> messages) const
        {
            std::string communication;

            for (const auto message : messages)
            {
                communication += message->json() + END_MESSAGE_FLAG;
            }

            return communication;
        }

        std::vector<std::shared_ptr<MessageWrapper>> deserialize(std::string currentCommunication)
        {
            auto wholeCommunication = _remainingDeserializationBuffer + currentCommunication;
            auto communications(StringsHelper::split(wholeCommunication, END_MESSAGE_FLAG));

            if (communications.back().empty() || communications.back().front() == '\0')
            {
                _remainingDeserializationBuffer.clear();
                communications.pop_back();
            }
            else
            {
                _remainingDeserializationBuffer = communications.back();
                communications.pop_back();
            }

            std::vector<std::shared_ptr<MessageWrapper>> messages;
            for (const auto communication : communications)
            {
                try
                {
                    messages.emplace_back(std::make_shared<MessageWrapper>(MessageWrapper::fromJson(communication)));
                }
                catch (...)
                {
                    // Parsing failed, there was a problem with a middle message
                    Logger::info("Discarded a message starting with : " + communication.substr(0, 100));
                }
            }

            return messages;
        }

    private:

        const std::string END_MESSAGE_FLAG = "\\n";

        std::string _remainingDeserializationBuffer;
    };
}