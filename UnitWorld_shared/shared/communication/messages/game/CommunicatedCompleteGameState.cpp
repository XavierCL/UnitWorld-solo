#include "CommunicatedCompleteGameState.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedCompleteGameState::CommunicatedCompleteGameState(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners):
    _players(players),
    _singuities(singuities),
    _spawners(spawners)
{}

nlohmann::json CommunicatedCompleteGameState::toJson() const
{
    std::vector<nlohmann::json> jsonPlayers;
    for (const auto& player : _players)
    {
        jsonPlayers.emplace_back(player.toJson());
    }

    std::vector<nlohmann::json> jsonSinguities;
    for (const auto& singuity : _singuities)
    {
        jsonSinguities.emplace_back(singuity.toJson());
    }

    std::vector<nlohmann::json> jsonSpawners;
    for (const auto& spawner : _spawners)
    {
        jsonSpawners.emplace_back(spawner.toJson());
    }

    nlohmann::json jsonData = {
        {"players", jsonPlayers},
        {"singuities", jsonSinguities},
        {"spawners", jsonSpawners}
    };

    return jsonData;
}

CommunicatedCompleteGameState CommunicatedCompleteGameState::fromJson(const nlohmann::json& parsedJson)
{
    std::vector<CommunicatedPlayer> players;
    for (const auto& parsedPlayer : parsedJson.at("players"))
    {
        players.emplace_back(CommunicatedPlayer::fromJson(parsedPlayer));
    }

    std::vector<CommunicatedSinguity> singuities;
    for (const auto& parsedSinguity : parsedJson.at("singuities"))
    {
        singuities.emplace_back(CommunicatedSinguity::fromJson(parsedSinguity));
    }

    std::vector<CommunicatedSpawner> spawners;
    for (const auto& parsedSpawner : parsedJson.at("spawners"))
    {
        spawners.emplace_back(CommunicatedSpawner::fromJson(parsedSpawner));
    }

    return CommunicatedCompleteGameState(players, singuities, spawners);
}