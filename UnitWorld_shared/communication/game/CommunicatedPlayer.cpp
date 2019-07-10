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

    return jsonData;
}