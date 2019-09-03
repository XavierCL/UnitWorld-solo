#include "CommunicatedSpawner.h"

#include <nlohmann/json.hpp>

using namespace uw;

std::string CommunicatedSpawner::toJson() const
{
    nlohmann::json allegenceData = _allegence.map<nlohmann::json>([](const CommunicatedSpawnerAllegence& allegence) {
        return nlohmann::json::parse(allegence.toJson());
    }).getOrElse([] { return nlohmann::json("none"); });

    nlohmann::json jsonData = {
        {"id", _id.str()},
        {"position", nlohmann::json::parse(_position.toJson())},
        {"allegence", allegenceData},
        {"last-spawn-timestamp", _lastSpawnTimestamp},
        {"total-spawned-count", _totalSpawnedCount}
    };

    return jsonData.dump();
}

CommunicatedSpawner CommunicatedSpawner::fromJson(const std::string& jsonData)
{
    nlohmann::json parsedJson(nlohmann::json::parse(jsonData));

    nlohmann::json parsedAllegence = parsedJson.at("allegence");

    return CommunicatedSpawner(
        xg::Guid(parsedJson.at("id").get<std::string>()),
        CommunicatedVector2D::fromJson(parsedJson.at("position").dump()),
        parsedAllegence.is_string()
            ? Options::None<CommunicatedSpawnerAllegence>()
            : Options::Some(CommunicatedSpawnerAllegence::fromJson(parsedAllegence.dump())),
        parsedJson.at("last-spawn-timestamp").get<unsigned long long>(),
        parsedJson.at("total-spawned-count").get<unsigned long long>()
    );
}
