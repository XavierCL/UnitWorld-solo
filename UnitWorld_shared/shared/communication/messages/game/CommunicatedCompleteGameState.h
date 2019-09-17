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
        CommunicatedCompleteGameState(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners);

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

        static CommunicatedCompleteGameState fromJson(const nlohmann::json& jsonData);

    private:

        std::vector<CommunicatedPlayer> _players;
        std::vector<CommunicatedSinguity> _singuities;
        std::vector<CommunicatedSpawner> _spawners;
    };
}