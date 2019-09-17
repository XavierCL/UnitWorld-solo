#include "CommunicatedSinguity.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedSinguity::CommunicatedSinguity(const xg::Guid& singuityId, const xg::Guid& playerId, const CommunicatedVector2D& position, const CommunicatedVector2D& speed, const Option<CommunicatedSinguityDestination>& destination, const double& healthPoint, const unsigned long long& lastShootTimestamp) :
    _singuityId(singuityId),
    _playerId(playerId),
    _position(position),
    _speed(speed),
    _destination(destination),
    _healthPoint(healthPoint),
    _lastShootTimestamp(lastShootTimestamp)
{}

nlohmann::json CommunicatedSinguity::toJson() const
{
    const auto singuityId = _singuityId.str();
    const auto playerId = _playerId.str();
    const auto position = _position.toJson();
    const auto speed = _speed.toJson();
    const auto destination = _destination
        .map<nlohmann::json>([](const CommunicatedSinguityDestination& destination) { return destination.toJson(); })
        .getOrElse(nlohmann::json("none"));

    nlohmann::json jsonData = {
        {"id", singuityId},
        {"playerId", playerId},
        {"position", position},
        {"speed", speed},
        {"destination", destination},
        {"health-points", _healthPoint},
        {"last-shoot-time", _lastShootTimestamp}
    };

    return jsonData;
}

CommunicatedSinguity CommunicatedSinguity::fromJson(const nlohmann::json& parsedJson)
{
    const auto destinationJson = parsedJson.at("destination");

    return CommunicatedSinguity(
        xg::Guid(parsedJson.at("id").get<std::string>()),
        xg::Guid(parsedJson.at("playerId").get<std::string>()),
        CommunicatedVector2D::fromJson(parsedJson.at("position")),
        CommunicatedVector2D::fromJson(parsedJson.at("speed")),
        destinationJson.is_string() && destinationJson.get<std::string>() == "none"
            ? Options::None<CommunicatedSinguityDestination>()
            : Options::Some(CommunicatedSinguityDestination::fromJson(destinationJson)),
        parsedJson.at("health-points").get<double>(),
        parsedJson.at("last-shoot-time").get<unsigned long long>()
    );
}

xg::Guid CommunicatedSinguity::singuityId() const
{
    return _singuityId;
}

xg::Guid CommunicatedSinguity::playerId() const
{
    return _playerId;
}

CommunicatedVector2D CommunicatedSinguity::position() const
{
    return _position;
}

CommunicatedVector2D CommunicatedSinguity::speed() const
{
    return _speed;
}

Option<CommunicatedSinguityDestination> CommunicatedSinguity::destination() const
{
    return _destination;
}

double CommunicatedSinguity::healthPoint() const
{
    return _healthPoint;
}

unsigned long long CommunicatedSinguity::lastShootTimestamp() const
{
    return _lastShootTimestamp;
}