#pragma once

#include "Message.h"

#include "game/CommunicatedCompleteGameState.h"

#include "commons/Guid.hpp"

namespace uw
{
    class CompleteGameStateMessage : public Message
    {
    public:
        CompleteGameStateMessage(const CommunicatedCompleteGameState& completeGameState, const xg::Guid& currentPlayerId);

        CompleteGameStateMessage(const std::string jsonData);

        virtual MessageType messageType() const;

        virtual std::string toJsonData() const;

        CommunicatedCompleteGameState completeGameState() const;

        xg::Guid getCurrentPlayerId() const;

    private:

        static CommunicatedCompleteGameState jsonDataToCompleteGameState(const std::string& jsonData);
        static xg::Guid jsonDataToCurrentPlayerId(const std::string& jsonData);

        const CommunicatedCompleteGameState _completeGameState;
        const xg::Guid _currentPlayerId;
    };
}