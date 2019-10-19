#include "CommunicatedSpawnerDestination.h"

#include <nlohmann/json.hpp>

using namespace uw;

nlohmann::json CommunicatedSpawnerDestination::toJson() const
{
    nlohmann::json parsedSpawnerAllegence = _spawnerAllegence
            .map<nlohmann::json>([](auto allegence) { return allegence.toJson(); })
            .getOrElse(nlohmann::json(NO_ALLEGENCE_VALUE));

    return nlohmann::json {
        {SPAWNER_ID_LABEL, _spawnerId.toBase64()},
        {SPAWNER_ALLEGENCE_LABEL, parsedSpawnerAllegence}
    };
}

CommunicatedSpawnerDestination CommunicatedSpawnerDestination::fromJson(const nlohmann::json& parsedJsonData)
{
    nlohmann::json jsonAllegence = parsedJsonData.at(SPAWNER_ALLEGENCE_LABEL);
    Option<CommunicatedSpawnerAllegence> communicatedSpawnerAllegence = jsonAllegence.is_string() && jsonAllegence.get<std::string>() == NO_ALLEGENCE_VALUE
        ? Options::None<CommunicatedSpawnerAllegence>()
        : Options::Some(CommunicatedSpawnerAllegence::fromJson(jsonAllegence));

    return CommunicatedSpawnerDestination(xg::Guid::fromBase64(parsedJsonData.at(SPAWNER_ID_LABEL).get<std::string>()), communicatedSpawnerAllegence);
}

const std::string CommunicatedSpawnerDestination::NO_ALLEGENCE_VALUE = "n";
const std::string CommunicatedSpawnerDestination::SPAWNER_ALLEGENCE_LABEL = "a";
const std::string CommunicatedSpawnerDestination::SPAWNER_ID_LABEL = "i";