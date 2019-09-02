#include "CommunicatedCompleteGameState.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedCompleteGameState::CommunicatedCompleteGameState(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners):
    _players(players),
    _singuities(singuities),
    _spawners(spawners)
{}

std::string CommunicatedCompleteGameState::toJson() const
{
    std::vector<nlohmann::json> jsonPlayers;
    for (const auto& player : _players)
    {
        jsonPlayers.emplace_back(nlohmann::json::parse(player.toJson()));
    }

    std::vector<nlohmann::json> jsonSinguities;
    for (const auto& singuity : _singuities)
    {
        jsonSinguities.emplace_back(nlohmann::json::parse(singuity.toJson()));
    }

    std::vector<nlohmann::json> jsonSpawners;
    for (const auto& spawner : _spawners)
    {
        jsonSpawners.emplace_back(nlohmann::json::parse(spawner.toJson()));
    }

    nlohmann::json jsonData = {
        {"players", jsonPlayers},
        {"singuities", jsonSinguities},
        {"spawners", jsonSpawners}
    };

    return jsonData;
}

CommunicatedCompleteGameState CommunicatedCompleteGameState::fromJson(const std::string& jsonData)
{
    nlohmann::json parsedJson(nlohmann::json::parse(jsonData));

    std::vector<CommunicatedPlayer> players;
    for (const auto& parsedPlayer : parsedJson.at("players"))
    {
        players.emplace_back(CommunicatedPlayer::fromJson(parsedPlayer.dump()));
    }

    std::vector<CommunicatedSinguity> singuities;
    for (const auto& parsedSinguity : parsedJson.at("singuities"))
    {
        players.emplace_back(CommunicatedSinguity::fromJson(parsedSinguity.dump()));
    }

    std::vector<CommunicatedSpawner> spawners;
    for (const auto& parsedSpawner : parsedJson.at("spawners"))
    {
        spawners.emplace_back(CommunicatedSpawner::fromJson(parsedSpawner.dump()));
    }

    return CommunicatedCompleteGameState(players, singuities, spawners);
}