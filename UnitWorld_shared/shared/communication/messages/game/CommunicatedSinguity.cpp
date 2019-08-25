#include "CommunicatedSinguity.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedSinguity::CommunicatedSinguity(const xg::Guid& singuityId, const xg::Guid& playerId, const CommunicatedVector2D& position, const CommunicatedVector2D& speed, const Option<CommunicatedVector2D>& destination, const bool& isBreakingForDestination, const double& healthPoint, const unsigned long long& lastShootTimestamp) :
    _singuityId(singuityId),
    _playerId(playerId),
    _position(position),
    _speed(speed),
    _destination(destination),
    _isBreakingForDestination(isBreakingForDestination),
    _healthPoint(healthPoint),
    _lastShootTimestamp(lastShootTimestamp)
{}

std::string CommunicatedSinguity::toJson() const
{
    const auto singuityId = _singuityId.str();
    const auto playerId = _playerId.str();
    const auto position = nlohmann::json::parse(_position.toJson());
    const auto speed = nlohmann::json::parse(_speed.toJson());
    const auto destination = _destination.map<nlohmann::json>([](const auto& destination) { return nlohmann::json::parse(destination.toJson()); }).getOrElse(nlohmann::json("none"));

    nlohmann::json jsonData = {
        {"id", singuityId},
        {"playerId", playerId},
        {"position", position},
        {"speed", speed},
        {"destination", destination},
        {"is-breaking-for-destination", _isBreakingForDestination},
        {"health-points", _healthPoint},
        {"last-shoot-time", _lastShootTimestamp}
    };

    return jsonData.dump();
}

CommunicatedSinguity CommunicatedSinguity::fromJson(const std::string& jsonData)
{
    nlohmann::json parsedJson = nlohmann::json::parse(jsonData);

    const auto destinationJson = parsedJson.at("destination").dump();

    return CommunicatedSinguity(
        xg::Guid(parsedJson.at("id").get<std::string>()),
        xg::Guid(parsedJson.at("playerId").get<std::string>()),
        CommunicatedVector2D::fromJson(parsedJson.at("position").dump()),
        CommunicatedVector2D::fromJson(parsedJson.at("speed").dump()),
        destinationJson == "\"none\""
            ? Option<CommunicatedVector2D>()
            : Option<CommunicatedVector2D>(CommunicatedVector2D::fromJson(destinationJson)),
        parsedJson.at("is-breaking-for-destination").get<bool>(),
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

Option<CommunicatedVector2D> CommunicatedSinguity::destination() const
{
    return _destination;
}

bool CommunicatedSinguity::isBreakingForDestination() const
{
    return _isBreakingForDestination;
}

double CommunicatedSinguity::healthPoint() const
{
    return _healthPoint;
}

unsigned long long CommunicatedSinguity::lastShootTimestamp() const
{
    return _lastShootTimestamp;
}