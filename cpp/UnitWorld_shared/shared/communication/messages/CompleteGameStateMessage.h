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

        static std::shared_ptr<CompleteGameStateMessage> fromJson(const nlohmann::json& jsonData);

        virtual MessageType messageType() const;

        virtual nlohmann::json toJsonData() const;

        CommunicatedCompleteGameState completeGameState() const;

        xg::Guid getCurrentPlayerId() const;

    private:

        const static std::string COMPLETE_GAME_STATE_LABEL;
        const static std::string CURRENT_PLAYER_ID_LABEL;

        const CommunicatedCompleteGameState _completeGameState;
        const xg::Guid _currentPlayerId;
    };
}