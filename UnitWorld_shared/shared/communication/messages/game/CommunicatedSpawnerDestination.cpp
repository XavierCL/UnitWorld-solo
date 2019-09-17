#include "CommunicatedSpawnerDestination.h"

#include <nlohmann/json.hpp>

using namespace uw;

nlohmann::json CommunicatedSpawnerDestination::toJson() const
{
    nlohmann::json parsedSpawnerAllegence = _spawnerAllegence
            .map<nlohmann::json>([](auto allegence) { return allegence.toJson(); })
            .getOrElse(nlohmann::json("none"));

    return nlohmann::json {
        {"spawner-id", _spawnerId.str()},
        {"spawner-allegence", parsedSpawnerAllegence}
    };
}

CommunicatedSpawnerDestination CommunicatedSpawnerDestination::fromJson(const nlohmann::json& parsedJsonData)
{
    nlohmann::json jsonAllegence = parsedJsonData.at("spawner-allegence");
    Option<CommunicatedSpawnerAllegence> communicatedSpawnerAllegence = jsonAllegence.is_string() && jsonAllegence.get<std::string>() == "none"
        ? Options::None<CommunicatedSpawnerAllegence>()
        : Options::Some(CommunicatedSpawnerAllegence::fromJson(jsonAllegence));

    return CommunicatedSpawnerDestination(xg::Guid(parsedJsonData.at("spawner-id").get<std::string>()), communicatedSpawnerAllegence);
}