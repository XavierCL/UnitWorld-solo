#include "CommunicatedCompleteGameState.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedCompleteGameState::CommunicatedCompleteGameState(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners, const unsigned long long& frameCount):
    _players(players),
    _singuities(singuities),
    _spawners(spawners),
    _frameCount(frameCount),
    _json(generateJson(players, singuities, spawners, frameCount))
{}

CommunicatedCompleteGameState::CommunicatedCompleteGameState(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const std::vector<CommunicatedSpawner>& spawners, const unsigned long long& frameCount, const nlohmann::json& json) :
    _players(players),
    _singuities(singuities),
    _spawners(spawners),
    _frameCount(frameCount),
    _json(json)
{}

nlohmann::json CommunicatedCompleteGameState::toJson() const
{
    return _json;
}

CommunicatedCompleteGameState CommunicatedCompleteGameState::fromJson(const nlohmann::json& parsedJson)
{
    std::vector<CommunicatedPlayer> players;
    for (const auto& parsedPlayer : parsedJson.at(PLAYERS_LABEL))
    {
        players.emplace_back(CommunicatedPlayer::fromJson(parsedPlayer));
    }

    std::vector<CommunicatedSinguity> singuities;
    for (const auto& parsedSinguity : parsedJson.at(SINGUITIES_LABEL))
    {
        singuities.emplace_back(CommunicatedSinguity::fromJson(parsedSinguity));
    }

    std::vector<CommunicatedSpawner> spawners;
    for (const auto& parsedSpawner : parsedJson.at(SPAWNERS_LABEL))
    {
        spawners.emplace_back(CommunicatedSpawner::fromJson(parsedSpawner));
    }

    return CommunicatedCompleteGameState(players, singuities, spawners, parsedJson.at(FRAME_COUNT_LABEL).get<unsigned long long>(), parsedJson);
}

const std::string CommunicatedCompleteGameState::PLAYERS_LABEL = "p";
const std::string CommunicatedCompleteGameState::SINGUITIES_LABEL = "s";
const std::string CommunicatedCompleteGameState::SPAWNERS_LABEL = "a";
const std::string CommunicatedCompleteGameState::FRAME_COUNT_LABEL = "f";