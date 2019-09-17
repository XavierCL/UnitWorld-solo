#include "CommunicatedSpawner.h"

#include <nlohmann/json.hpp>

using namespace uw;

nlohmann::json CommunicatedSpawner::toJson() const
{
    nlohmann::json allegenceData = _allegence.map<nlohmann::json>([](const CommunicatedSpawnerAllegence& allegence) {
        return allegence.toJson();
    }).getOrElse([] { return nlohmann::json("none"); });

    nlohmann::json jsonData = {
        {"id", _id.str()},
        {"position", _position.toJson()},
        {"allegence", allegenceData},
        {"last-spawn-timestamp", _lastSpawnTimestamp},
        {"total-spawned-count", _totalSpawnedCount}
    };

    return jsonData.dump();
}

CommunicatedSpawner CommunicatedSpawner::fromJson(const nlohmann::json& parsedJson)
{
    nlohmann::json parsedAllegence = parsedJson.at("allegence");

    return CommunicatedSpawner(
        xg::Guid(parsedJson.at("id").get<std::string>()),
        CommunicatedVector2D::fromJson(parsedJson.at("position")),
        parsedAllegence.is_string()
            ? Options::None<CommunicatedSpawnerAllegence>()
            : Options::Some(CommunicatedSpawnerAllegence::fromJson(parsedAllegence)),
        parsedJson.at("last-spawn-timestamp").get<unsigned long long>(),
        parsedJson.at("total-spawned-count").get<unsigned long long>()
    );
}
