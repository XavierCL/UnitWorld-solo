#include "CommunicatedPlayer.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedPlayer::CommunicatedPlayer(const xg::Guid& playerId) :
    _playerId(playerId)
{}

nlohmann::json CommunicatedPlayer::toJson() const
{
    nlohmann::json jsonData = {
        {PLAYER_ID_LABEL, _playerId.str()}
    };

    return jsonData;
}

CommunicatedPlayer CommunicatedPlayer::fromJson(const nlohmann::json& parsedJson)
{   
    return CommunicatedPlayer(xg::Guid(parsedJson.at(PLAYER_ID_LABEL).get<std::string>()));
}

xg::Guid CommunicatedPlayer::playerId() const
{
    return _playerId;
}

const std::string CommunicatedPlayer::PLAYER_ID_LABEL = "i";