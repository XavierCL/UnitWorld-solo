#include "CommunicatedSpawner.h"

#include <nlohmann/json.hpp>

using namespace uw;

nlohmann::json CommunicatedSpawner::toJson() const
{
    nlohmann::json allegenceData = _allegence.map<nlohmann::json>([](const CommunicatedSpawnerAllegence& allegence) {
        return allegence.toJson();
    }).getOrElse([] { return nlohmann::json(NO_ALLEGENCE_VALUE); });

    nlohmann::json jsonData = {
        {SPAWNER_ID_LABEL, _id.str()},
        {POSITION_LABEL, _position.toJson()},
        {ALLEGENCE_LABEL, allegenceData},
        {LAST_SPAWN_FRAME_COUNT_LABEL, _lastSpawnFrameCount},
        {TOTAL_SPAWNED_COUNT_LABEL, _totalSpawnedCount}
    };

    return jsonData;
}

CommunicatedSpawner CommunicatedSpawner::fromJson(const nlohmann::json& parsedJson)
{
    nlohmann::json parsedAllegence = parsedJson.at(ALLEGENCE_LABEL);

    return CommunicatedSpawner(
        xg::Guid(parsedJson.at(SPAWNER_ID_LABEL).get<std::string>()),
        CommunicatedVector2D::fromJson(parsedJson.at(POSITION_LABEL)),
        parsedAllegence.is_string() && parsedAllegence.get<std::string>() == NO_ALLEGENCE_VALUE
            ? Options::None<CommunicatedSpawnerAllegence>()
            : Options::Some(CommunicatedSpawnerAllegence::fromJson(parsedAllegence)),
        parsedJson.at(LAST_SPAWN_FRAME_COUNT_LABEL).get<unsigned long long>(),
        parsedJson.at(TOTAL_SPAWNED_COUNT_LABEL).get<unsigned long long>()
    );
}

const std::string CommunicatedSpawner::NO_ALLEGENCE_VALUE = "n";
const std::string CommunicatedSpawner::SPAWNER_ID_LABEL = "i";
const std::string CommunicatedSpawner::POSITION_LABEL = "p";
const std::string CommunicatedSpawner::ALLEGENCE_LABEL = "a";
const std::string CommunicatedSpawner::LAST_SPAWN_FRAME_COUNT_LABEL = "l";
const std::string CommunicatedSpawner::TOTAL_SPAWNED_COUNT_LABEL = "t";