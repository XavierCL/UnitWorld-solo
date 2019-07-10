#pragma once

#include "Message.h"

#include "game/CommunicatedPlayer.h"
#include "game/CommunicatedSinguity.h"

namespace uw
{
    class CompleteGameStateMessage : public Message
    {
    public:
        CompleteGameStateMessage(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities):
            _players(players),
            _singuities(singuities)
        {}

        virtual MessageType messageType() const
        {
            return MessageType::CompleteGameStateMessageType;
        }

        virtual std::string toJsonData() const
        {
            std::vector<std::string> playerJsons;
            for (const auto& communicatedPlayer : _players)
            {
                playerJsons.emplace_back(communicatedPlayer.toJson());
            }

            std::vector<std::string> singuityJsons;
            for (const auto& communicatedSinguity : _singuities)
            {
                singuityJsons.emplace_back(communicatedSinguity.toJson());
            }

            nlohmann::json jsonData = {
                {"players", playerJsons},
                {"singuities", singuityJsons}
            };

            return jsonData;
        }

    private:

        std::vector<CommunicatedPlayer> _players;
        std::vector<CommunicatedSinguity> _singuities;
    };
}