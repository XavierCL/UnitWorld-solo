#include "MoveMobileUnitsToPositionMessage.h"

#include <nlohmann/json.hpp>

using namespace uw;

MoveMobileUnitsToPositionMessage::MoveMobileUnitsToPositionMessage(const std::vector<xg::Guid>& singuityIds, const CommunicatedVector2D& destination) :
    _singuityIds(singuityIds),
    _destination(destination)
{}

std::shared_ptr<MoveMobileUnitsToPositionMessage> MoveMobileUnitsToPositionMessage::fromJson(const std::string& json)
{
    nlohmann::json parsedData = nlohmann::json::parse(json);

    std::vector<xg::Guid> singuityIds;
    for (const auto singuityId : parsedData.at("mobile-units-id"))
    {
        singuityIds.emplace_back(singuityId.get<std::string>());
    }

    return std::make_shared<MoveMobileUnitsToPositionMessage>(
        std::move(singuityIds),
        CommunicatedVector2D::fromJson(parsedData.at("destination-position"))
    );
}

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

    nlohmann::json parsedDestination(_destination.toJson());

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