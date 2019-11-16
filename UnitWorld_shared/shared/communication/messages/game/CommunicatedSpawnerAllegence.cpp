#include "CommunicatedSpawnerAllegence.h"

#include <nlohmann/json.hpp>

using namespace uw;

nlohmann::json CommunicatedSpawnerAllegence::toJson() const
{
    nlohmann::json jsonData = {
        {"is-claimed", _isClaimed},
        {"health-point", _healthPoint},
        {"player-id", _playerId.str()}
    };

    return jsonData;
}

CommunicatedSpawnerAllegence CommunicatedSpawnerAllegence::fromJson(const nlohmann::json& parsedJson)
{
    return CommunicatedSpawnerAllegence(
        parsedJson.at("is-claimed").get<bool>(),
        parsedJson.at("health-point").get<double>(),
        xg::Guid(parsedJson.at("player-id").get<std::string>())
    );
}