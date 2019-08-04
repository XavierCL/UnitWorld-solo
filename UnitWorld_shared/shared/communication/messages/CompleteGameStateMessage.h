#pragma once

#include "Message.h"

#include "game/CommunicatedPlayer.h"
#include "game/CommunicatedSinguity.h"

#include "commons/Guid.hpp"

namespace uw
{
    class CompleteGameStateMessage : public Message
    {
    public:
        CompleteGameStateMessage(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const xg::Guid& currentPlayerId);

        CompleteGameStateMessage(const std::string jsonData);

        virtual MessageType messageType() const;

        virtual std::string toJsonData() const;

        std::vector<CommunicatedPlayer> getPlayers() const;

        std::vector<CommunicatedSinguity> getSinguities() const;

        xg::Guid getCurrentPlayerId() const;

    private:

        static std::vector<CommunicatedPlayer> jsonDataToPlayers(const std::string& jsonData);
        static std::vector<CommunicatedSinguity> jsonDataToSinguities(const std::string& jsonData);
        static xg::Guid jsonDataToCurrentPlayerId(const std::string& jsonData);

        const std::vector<CommunicatedPlayer> _players;
        const std::vector<CommunicatedSinguity> _singuities;
        const xg::Guid _currentPlayerId;
    };
}