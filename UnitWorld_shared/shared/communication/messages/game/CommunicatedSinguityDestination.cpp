#include "CommunicatedSinguityDestination.h"

#include "commons/CollectionPipe.h"

#include <nlohmann/json.hpp>

using namespace uw;

std::string CommunicatedSinguityDestination::toJson() const
{
    return std::visit(overloaded{
        [](const CommunicatedVector2D& point) {
            return nlohmann::json {{"point", nlohmann::json::parse(point.toJson())}};
        },
        [](const CommunicatedSpawnerDestination& spawnerDestination) {
            return nlohmann::json {{"spawner", nlohmann::json::parse(spawnerDestination.toJson())}};
        }
    }, _destination).dump();
}

CommunicatedSinguityDestination CommunicatedSinguityDestination::fromJson(const std::string& jsonData)
{
    nlohmann::json parsedData = nlohmann::json::parse(jsonData);

    if (parsedData.contains("point"))
    {
        return CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination> {CommunicatedVector2D::fromJson(parsedData.at("point").dump())});
    }
    else
    {
        return CommunicatedSinguityDestination(std::variant<CommunicatedVector2D, CommunicatedSpawnerDestination> {CommunicatedSpawnerDestination::fromJson(parsedData.at("spawner").dump())});
    }
}