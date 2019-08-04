#include "CompleteGameStateMessage.h"

#include <nlohmann/json.hpp>

#include <chrono>

using namespace uw;

CompleteGameStateMessage::CompleteGameStateMessage(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities, const xg::Guid& currentPlayerId) :
    _players(players),
    _singuities(singuities),
    _currentPlayerId(currentPlayerId)
{}

CompleteGameStateMessage::CompleteGameStateMessage(const std::string jsonData) :
    _players(jsonDataToPlayers(jsonData)),
    _singuities(jsonDataToSinguities(jsonData)),
    _currentPlayerId(jsonDataToCurrentPlayerId(jsonData))
{}

MessageType CompleteGameStateMessage::messageType() const
{
    return MessageType::CompleteGameStateMessageType;
}

std::string CompleteGameStateMessage::toJsonData() const
{
    nlohmann::json playerJsons;
    for (const auto& communicatedPlayer : _players)
    {
        playerJsons.emplace_back(nlohmann::json::parse(communicatedPlayer.toJson()));
    }

    nlohmann::json singuityJsons;
    for (const auto& communicatedSinguity : _singuities)
    {
        singuityJsons.emplace_back(nlohmann::json::parse(communicatedSinguity.toJson()));
    }

    nlohmann::json jsonData = {
        {"players", playerJsons},
        {"singuities", singuityJsons},
        {"currentPlayerId", _currentPlayerId.str()}
    };

    return jsonData.dump();
}

std::vector<CommunicatedPlayer> CompleteGameStateMessage::getPlayers() const
{
    return _players;
}

std::vector<CommunicatedSinguity> CompleteGameStateMessage::getSinguities() const
{
    return _singuities;
}

xg::Guid CompleteGameStateMessage::getCurrentPlayerId() const
{
    return _currentPlayerId;
}

std::vector<CommunicatedPlayer> CompleteGameStateMessage::jsonDataToPlayers(const std::string& jsonData)
{
    nlohmann::json parsedData = nlohmann::json::parse(jsonData);

    std::vector<CommunicatedPlayer> players;
    for (const auto player : parsedData.at("players"))
    {
        players.emplace_back(CommunicatedPlayer::fromJson(player.dump()));
    }

    return players;
}

std::vector<CommunicatedSinguity> CompleteGameStateMessage::jsonDataToSinguities(const std::string& jsonData)
{
    nlohmann::json parsedData = nlohmann::json::parse(jsonData);

    std::vector<CommunicatedSinguity> singuities;
    for (const auto singuity : parsedData.at("singuities"))
    {
        singuities.emplace_back(CommunicatedSinguity::fromJson(singuity.dump()));
    }

    return singuities;
}

xg::Guid CompleteGameStateMessage::jsonDataToCurrentPlayerId(const std::string& jsonData)
{
    nlohmann::json parsedData = nlohmann::json::parse(jsonData);

    return xg::Guid(parsedData.at("currentPlayerId").get<std::string>());
}