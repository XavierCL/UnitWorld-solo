#include "CommunicatedSinguityDestination.h"

#include "commons/CollectionPipe.h"

#include <nlohmann/json.hpp>

using namespace uw;

nlohmann::json CommunicatedSinguityDestination::toJson() const
{
    return std::visit(overloaded{
        [](const CommunicatedVector2D& point) {
            return nlohmann::json {{POINT_LABEL, point.toJson()}};
        },
        [](const CommunicatedSpawnerDestination& spawnerDestination) {
            return nlohmann::json {{SPAWNER_LABEL, spawnerDestination.toJson()}};
        }
    }, _destination);
}

CommunicatedSinguityDestination CommunicatedSinguityDestination::fromJson(const nlohmann::json& parsedData)
{
    if (parsedData.contains("point"))
    {
        return CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination> {CommunicatedVector2D::fromJson(parsedData.at("point"))});
    }
    else
    {
        return CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination> {CommunicatedSpawnerDestination::fromJson(parsedData.at("spawner"))});
    }
}

const std::string CommunicatedSinguityDestination::POINT_LABEL = "p";
const std::string CommunicatedSinguityDestination::SPAWNER_LABEL = "s";