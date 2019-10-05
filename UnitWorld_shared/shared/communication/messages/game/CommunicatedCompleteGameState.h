#pragma once

#include "CommunicatedPlayer.h"
#include "CommunicatedSinguity.h"
#include "CommunicatedSpawner.h"

#include <nlohmann/json.hpp>

#include <vector>

namespace uw
{
    class CommunicatedCompleteGameState
    {
    public:
        CommunicatedCompleteGameState(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners, const unsigned long long& frameCount);

        nlohmann::json toJson() const;

        const std::vector<CommunicatedSpawner>& spawners() const
        {
            return _spawners;
        }

        const std::vector<CommunicatedPlayer>& players() const
        {
            return _players;
        }

        const std::vector<CommunicatedSinguity>& singuities() const
        {
            return _singuities;
        }

        const unsigned long long frameCount() const
        {
            return _frameCount;
        }

        static CommunicatedCompleteGameState fromJson(const nlohmann::json& jsonData);

    private:

        const static std::string PLAYERS_LABEL;
        const static std::string SINGUITIES_LABEL;
        const static std::string SPAWNERS_LABEL;
        const static std::string FRAME_COUNT_LABEL;

        std::vector<CommunicatedPlayer> _players;
        std::vector<CommunicatedSinguity> _singuities;
        std::vector<CommunicatedSpawner> _spawners;
        unsigned long long _frameCount;
    };
}