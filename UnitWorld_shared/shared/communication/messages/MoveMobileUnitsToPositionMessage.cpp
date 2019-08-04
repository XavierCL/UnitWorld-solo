#include "MoveMobileUnitsToPositionMessage.h"

#include <nlohmann/json.hpp>

using namespace uw;

MoveMobileUnitsToPositionMessage::MoveMobileUnitsToPositionMessage(const std::vector<xg::Guid>& singuityIds, const CommunicatedVector2D& destination) :
    _singuityIds(singuityIds),
    _destination(destination)
{}

MessageType MoveMobileUnitsToPositionMessage::messageType() const
{
    return MessageType::MoveMobileUnitsToPositionMessageType;
}

std::string MoveMobileUnitsToPositionMessage::toJsonData() const
{
    std::vector<std::string> singuityStringIds;
    for (const auto singuityId : _singuityIds)
    {
        singuityStringIds.emplace_back(singuityId.str());
    }

    nlohmann::json parsedDestination(nlohmann::json::parse(_destination.toJson()));

    nlohmann::json returnedJson({
        {"mobile-units-id", singuityStringIds},
        {"destination-position", parsedDestination}
        });

    return returnedJson.dump();
}

std::vector<xg::Guid> MoveMobileUnitsToPositionMessage::singuityIds() const
{
    return _singuityIds;
}

CommunicatedVector2D MoveMobileUnitsToPositionMessage::destination() const
{
    return _destination;
}