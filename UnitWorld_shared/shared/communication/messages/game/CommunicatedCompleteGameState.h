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
        CommunicatedCompleteGameState(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners, const long long& frameCount);
        CommunicatedCompleteGameState(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners, const long long& frameCount, const nlohmann::json& json);

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

        const long long frameCount() const
        {
            return _frameCount;
        }

        static CommunicatedCompleteGameState fromJson(const nlohmann::json& jsonData);

    private:

        static nlohmann::json generateJson(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners, const long long& frameCount)
        {
            std::vector<nlohmann::json> jsonPlayers;
            for (const auto& player : players)
            {
                jsonPlayers.emplace_back(player.toJson());
            }

            std::vector<nlohmann::json> jsonSinguities;
            for (const auto& singuity : singuities)
            {
                jsonSinguities.emplace_back(singuity.toJson());
            }

            std::vector<nlohmann::json> jsonSpawners;
            for (const auto& spawner : spawners)
            {
                jsonSpawners.emplace_back(spawner.toJson());
            }

            nlohmann::json jsonData = {
                {PLAYERS_LABEL, jsonPlayers},
                {SINGUITIES_LABEL, jsonSinguities},
                {SPAWNERS_LABEL, jsonSpawners},
                {FRAME_COUNT_LABEL, frameCount}
            };

            return jsonData;
        }

        const static std::string PLAYERS_LABEL;
        const static std::string SINGUITIES_LABEL;
        const static std::string SPAWNERS_LABEL;
        const static std::string FRAME_COUNT_LABEL;

        std::vector<CommunicatedPlayer> _players;
        std::vector<CommunicatedSinguity> _singuities;
        std::vector<CommunicatedSpawner> _spawners;
        long long _frameCount;

        nlohmann::json _json;
    };
}