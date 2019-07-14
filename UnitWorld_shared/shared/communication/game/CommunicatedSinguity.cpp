#include "CommunicatedSinguity.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedSinguity::CommunicatedSinguity(const xg::Guid& singuityId, const xg::Guid& playerId, const CommunicatedVector2D& position, const CommunicatedVector2D& speed, const Option<CommunicatedVector2D>& destination) :
    _singuityId(singuityId),
    _playerId(playerId),
    _position(position),
    _speed(speed),
    _destination(destination)
{}

std::string CommunicatedSinguity::toJson() const
{
    nlohmann::json jsonData = {
        {"id", _singuityId.str()},
        {"playerId", _playerId.str()},
        {"position", _position.toJson()},
        {"speed", _speed.toJson()},
        {"destination", _destination.map<std::string>([](const auto& destination) { return destination.toJson(); }).getOrElse(std::string("none"))}
    };

    return jsonData;
}

CommunicatedSinguity CommunicatedSinguity::fromJson(const std::string& jsonData)
{
    nlohmann::json parsedJson = nlohmann::json::parse(jsonData);

    return CommunicatedSinguity(
        xg::Guid(parsedJson.at("id").get<std::string>()),
        xg::Guid(parsedJson.at("playerId").get<std::string>()),
        CommunicatedVector2D::fromJson(parsedJson.at("position")),
        CommunicatedVector2D::fromJson(parsedJson.at("speed")),
        CommunicatedVector2D::fromJson(parsedJson.at("destination"))
    );
}