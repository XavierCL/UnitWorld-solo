#include "CommunicatedSpawner.h"

#include <nlohmann/json.hpp>

using namespace uw;

nlohmann::json CommunicatedSpawner::toJson() const
{
    nlohmann::json allegenceData = _allegence.map<nlohmann::json>([](const CommunicatedSpawnerAllegence& allegence) {
        return allegence.toJson();
    }).getOrElse([] { return nlohmann::json(NO_ALLEGENCE_VALUE); });

    nlohmann::json rallyData = _rally.map<nlohmann::json>([](const CommunicatedSinguityDestination& rally) {
        return rally.toJson();
    }).getOrElse([] { return nlohmann::json(NO_RALLY_VALUE); });

    nlohmann::json jsonData = {
        {SPAWNER_ID_LABEL, _id.toBase64()},
        {POSITION_LABEL, _position.toJson()},
        {ALLEGENCE_LABEL, allegenceData},
        {RALLY_LABEL, rallyData},
        {LAST_SPAWN_FRAME_COUNT_LABEL, _lastSpawnFrameCount},
        {TOTAL_SPAWNED_COUNT_LABEL, _totalSpawnedCount},
        {LAST_CLAIMED_FRAME_COUNT_LABEL, _lastClaimedFrameCount}
    };

    return jsonData;
}

CommunicatedSpawner CommunicatedSpawner::fromJson(const nlohmann::json& parsedJson)
{
    nlohmann::json parsedAllegence = parsedJson.at(ALLEGENCE_LABEL);
    nlohmann::json parsedRally = parsedJson.at(RALLY_LABEL);

    return CommunicatedSpawner(
        xg::Guid::fromBase64(parsedJson.at(SPAWNER_ID_LABEL).get<std::string>()),
        CommunicatedVector2D::fromJson(parsedJson.at(POSITION_LABEL)),
        parsedAllegence.is_string() && parsedAllegence.get<std::string>() == NO_ALLEGENCE_VALUE
            ? Options::None<CommunicatedSpawnerAllegence>()
            : Options::Some(CommunicatedSpawnerAllegence::fromJson(parsedAllegence)),
        parsedRally.is_string() && parsedRally.get<std::string>() == NO_RALLY_VALUE
            ? Options::None<CommunicatedSinguityDestination>()
            : Options::Some(CommunicatedSinguityDestination::fromJson(parsedRally)),
        parsedJson.at(LAST_SPAWN_FRAME_COUNT_LABEL).get<long long>(),
        parsedJson.at(TOTAL_SPAWNED_COUNT_LABEL).get<long long>(),
        parsedJson.at(LAST_CLAIMED_FRAME_COUNT_LABEL).get<long long>()
    );
}

const std::string CommunicatedSpawner::NO_ALLEGENCE_VALUE = "";
const std::string CommunicatedSpawner::NO_RALLY_VALUE = "";
const std::string CommunicatedSpawner::SPAWNER_ID_LABEL = "i";
const std::string CommunicatedSpawner::POSITION_LABEL = "p";
const std::string CommunicatedSpawner::ALLEGENCE_LABEL = "a";
const std::string CommunicatedSpawner::RALLY_LABEL = "r";
const std::string CommunicatedSpawner::LAST_SPAWN_FRAME_COUNT_LABEL = "l";
const std::string CommunicatedSpawner::TOTAL_SPAWNED_COUNT_LABEL = "t";
const std::string CommunicatedSpawner::LAST_CLAIMED_FRAME_COUNT_LABEL = "c";