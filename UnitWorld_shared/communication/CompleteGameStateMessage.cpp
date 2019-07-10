#include "CompleteGameStateMessage.h"

#include <nlohmann/json.hpp>

using namespace uw;

CompleteGameStateMessage::CompleteGameStateMessage(const std::vector<CommunicatedPlayer>& players, const std::vector<CommunicatedSinguity>& singuities) :
    _players(players),
    _singuities(singuities)
{}

// TODO: parse the json data
CompleteGameStateMessage::CompleteGameStateMessage(const std::string jsonData) :
    _players(),
    _singuities()
{}

MessageType CompleteGameStateMessage::messageType() const
{
    return MessageType::CompleteGameStateMessageType;
}

std::string CompleteGameStateMessage::toJsonData() const
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