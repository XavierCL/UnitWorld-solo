#include "CommunicatedPlayer.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedPlayer::CommunicatedPlayer(const xg::Guid& playerId) :
    _playerId(playerId)
{}

nlohmann::json CommunicatedPlayer::toJson() const
{
    nlohmann::json jsonData = {
        {"id", _playerId.str()}
    };

    return jsonData;
}

CommunicatedPlayer CommunicatedPlayer::fromJson(const nlohmann::json& parsedJson)
{   
    return CommunicatedPlayer(xg::Guid(parsedJson.at("id").get<std::string>()));
}

xg::Guid CommunicatedPlayer::playerId() const
{
    return _playerId;
}