#include "CommunicatedSpawnerAllegence.h"

#include <nlohmann/json.hpp>

using namespace uw;

std::string CommunicatedSpawnerAllegence::toJson() const
{
    nlohmann::json jsonData = {
        {"is-claimed", _isClaimed},
        {"health-point", _healthPoint},
        {"player-id", _playerId.str()}
    };

    return jsonData.dump();
}

CommunicatedSpawnerAllegence CommunicatedSpawnerAllegence::fromJson(const std::string& jsonData)
{
    nlohmann::json parsedJson = nlohmann::json::parse(jsonData);

    return CommunicatedSpawnerAllegence(
        parsedJson.at("is-claimed").get<bool>(),
        parsedJson.at("health-point").get<double>(),
        xg::Guid(parsedJson.at("player-id").get<std::string>())
    );
}