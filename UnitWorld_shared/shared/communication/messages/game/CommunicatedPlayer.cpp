#include "CommunicatedPlayer.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedPlayer::CommunicatedPlayer(const xg::Guid& playerId) :
    _playerId(playerId)
{}

std::string CommunicatedPlayer::toJson() const
{
    nlohmann::json jsonData = {
        {"id", _playerId.str()}
    };

    return jsonData.dump();
}

CommunicatedPlayer CommunicatedPlayer::fromJson(std::string jsonData)
{
    nlohmann::json parsedJson = nlohmann::json::parse(jsonData);
    
    return CommunicatedPlayer(xg::Guid(parsedJson.at("id").get<std::string>()));
}

xg::Guid CommunicatedPlayer::playerId() const
{
    return _playerId;
}