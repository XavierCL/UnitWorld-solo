#include "MoveMobileUnitsToSpawnerMessage.h"

#include <nlohmann/json.hpp>

using namespace uw;

MoveMobileUnitsToSpawnerMessage::MoveMobileUnitsToSpawnerMessage(const std::vector<xg::Guid>& singuityIds, const xg::Guid& spawnerId) :
    _singuityIds(singuityIds),
    _spawnerId(spawnerId)
{}

std::shared_ptr<MoveMobileUnitsToSpawnerMessage> MoveMobileUnitsToSpawnerMessage::fromJson(const nlohmann::json& json)
{
    std::vector<xg::Guid> singuityIds;
    for (const auto singuityId : json.at(MOBILE_UNITS_ID_LABEL))
    {
        singuityIds.emplace_back(xg::Guid::fromBase64(singuityId.get<std::string>()));
    }

    return std::make_shared<MoveMobileUnitsToSpawnerMessage>(
        std::move(singuityIds),
        xg::Guid::fromBase64(json.at(SPAWNER_ID_LABEL).get<std::string>())
    );
}

MessageType MoveMobileUnitsToSpawnerMessage::messageType() const
{
    return MessageType::MoveMobileUnitsToSpawnerMessageType;
}

nlohmann::json MoveMobileUnitsToSpawnerMessage::toJsonData() const
{
    std::vector<std::string> singuityStringIds;
    for (const auto singuityId : _singuityIds)
    {
        singuityStringIds.emplace_back(singuityId.toBase64());
    }

    nlohmann::json returnedJson({
        {MOBILE_UNITS_ID_LABEL, singuityStringIds},
        {SPAWNER_ID_LABEL, _spawnerId.toBase64()}
    });

    return returnedJson;
}

std::vector<xg::Guid> MoveMobileUnitsToSpawnerMessage::singuityIds() const
{
    return _singuityIds;
}

xg::Guid MoveMobileUnitsToSpawnerMessage::spawnerId() const
{
    return _spawnerId;
}

const std::string MoveMobileUnitsToSpawnerMessage::MOBILE_UNITS_ID_LABEL = "m";
const std::string MoveMobileUnitsToSpawnerMessage::SPAWNER_ID_LABEL = "s";