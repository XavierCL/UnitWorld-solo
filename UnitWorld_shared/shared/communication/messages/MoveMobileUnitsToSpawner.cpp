#include "MoveMobileUnitsToSpawnerMessage.h"

#include <nlohmann/json.hpp>

using namespace uw;

MoveMobileUnitsToSpawnerMessage::MoveMobileUnitsToSpawnerMessage(const std::vector<xg::Guid>& singuityIds, const xg::Guid& spawnerId) :
    _singuityIds(singuityIds),
    _spawnerId(spawnerId)
{}

std::shared_ptr<MoveMobileUnitsToSpawnerMessage> MoveMobileUnitsToSpawnerMessage::fromJson(const std::string& json)
{
    nlohmann::json parsedData = nlohmann::json::parse(json);

    std::vector<xg::Guid> singuityIds;
    for (const auto singuityId : parsedData.at("mobile-units-id"))
    {
        singuityIds.emplace_back(singuityId.get<std::string>());
    }

    return std::make_shared<MoveMobileUnitsToSpawnerMessage>(
        std::move(singuityIds),
        xg::Guid(std::move(parsedData).at("spawner-id").get<std::string>())
    );
}

MessageType MoveMobileUnitsToSpawnerMessage::messageType() const
{
    return MessageType::MoveMobileUnitsToSpawnerMessageType;
}

std::string MoveMobileUnitsToSpawnerMessage::toJsonData() const
{
    std::vector<std::string> singuityStringIds;
    for (const auto singuityId : _singuityIds)
    {
        singuityStringIds.emplace_back(singuityId.str());
    }

    nlohmann::json returnedJson({
        {"mobile-units-id", singuityStringIds},
        {"destination-position", _spawnerId.str()}
    });

    return returnedJson.dump();
}

std::vector<xg::Guid> MoveMobileUnitsToSpawnerMessage::singuityIds() const
{
    return _singuityIds;
}

xg::Guid MoveMobileUnitsToSpawnerMessage::spawnerId() const
{
    return _spawnerId;
}