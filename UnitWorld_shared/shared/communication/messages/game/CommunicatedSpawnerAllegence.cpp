#include "CommunicatedSpawnerAllegence.h"

#include <nlohmann/json.hpp>

using namespace uw;

nlohmann::json CommunicatedSpawnerAllegence::toJson() const
{
    nlohmann::json jsonData = {
        {IS_CLAIMED_LABEL, _isClaimed},
        {HEALTH_POINTS_LABEL, _healthPoint},
        {PLAYER_ID_LABEL, _playerId.str()}
    };

    return jsonData;
}

CommunicatedSpawnerAllegence CommunicatedSpawnerAllegence::fromJson(const nlohmann::json& parsedJson)
{
    return CommunicatedSpawnerAllegence(
        parsedJson.at(IS_CLAIMED_LABEL).get<bool>(),
        parsedJson.at(HEALTH_POINTS_LABEL).get<double>(),
        xg::Guid(parsedJson.at(PLAYER_ID_LABEL).get<std::string>())
    );
}

const std::string CommunicatedSpawnerAllegence::IS_CLAIMED_LABEL = "i";
const std::string CommunicatedSpawnerAllegence::HEALTH_POINTS_LABEL = "h";
const std::string CommunicatedSpawnerAllegence::PLAYER_ID_LABEL = "p";