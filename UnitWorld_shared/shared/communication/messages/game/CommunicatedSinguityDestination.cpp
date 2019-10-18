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
        },
        [](const xg::Guid& unconditionalSpawnerDestination) {
            return nlohmann::json {{UNCONDITIONAL_SPAWNER_LABEL, unconditionalSpawnerDestination.str()}};
        }
    }, _destination);
}

CommunicatedSinguityDestination CommunicatedSinguityDestination::fromJson(const nlohmann::json& parsedData)
{
    if (parsedData.contains(POINT_LABEL))
    {
        return CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination, xg::Guid> {CommunicatedVector2D::fromJson(parsedData.at(POINT_LABEL))});
    }
    else if (parsedData.contains(SPAWNER_LABEL))
    {
        return CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination, xg::Guid> {CommunicatedSpawnerDestination::fromJson(parsedData.at(SPAWNER_LABEL))});
    }
    else if (parsedData.contains(UNCONDITIONAL_SPAWNER_LABEL))
    {
        return CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination, xg::Guid> {xg::Guid(parsedData.at(UNCONDITIONAL_SPAWNER_LABEL).get<std::string>())});
    }
}

const std::string CommunicatedSinguityDestination::POINT_LABEL = "p";
const std::string CommunicatedSinguityDestination::SPAWNER_LABEL = "s";
const std::string CommunicatedSinguityDestination::UNCONDITIONAL_SPAWNER_LABEL = "u";