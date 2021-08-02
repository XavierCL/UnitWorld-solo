#include "CommunicatedSinguity.h"

#include "commons/StringsHelper.h"

#include <nlohmann/json.hpp>

using namespace uw;

CommunicatedSinguity::CommunicatedSinguity(const xg::Guid& singuityId, const xg::Guid& playerId, const CommunicatedVector2D& position, const CommunicatedVector2D& speed, const Option<CommunicatedSinguityDestination>& destination, const double& healthPoint, const long long& lastShootFrameCount) :
    _singuityId(singuityId),
    _playerId(playerId),
    _position(position),
    _speed(speed),
    _destination(destination),
    _healthPoint(healthPoint),
    _lastShootFameCount(lastShootFrameCount)
{}

nlohmann::json CommunicatedSinguity::toJson() const
{
    const auto singuityId = _singuityId.toBase64();
    const auto playerId = _playerId.toBase64();
    const auto position = _position.toJson();
    const auto speed = _speed.toJson();
    const auto destination = _destination
        .map<nlohmann::json>([](const CommunicatedSinguityDestination& destination) { return destination.toJson(); })
        .getOrElse(nlohmann::json(NO_DESTINATION_VALUE));

    nlohmann::json jsonData = {
        {SINGUITY_ID_LABEL, singuityId},
        {PLAYER_ID_LABEL, playerId},
        {POSITION_LABEL, position},
        {SPEED_LABEL, speed},
        {DESTINATION_LABEL, destination},
        {HEALTH_POINTS_LABEL, _healthPoint},
        {LAST_SHOOT_FRAME_COUNT_LABEL, _lastShootFameCount}
    };

    return jsonData;
}

CommunicatedSinguity CommunicatedSinguity::fromJson(const nlohmann::json& parsedJson)
{
    const auto destinationJson = parsedJson.at(DESTINATION_LABEL);

    return CommunicatedSinguity(
        xg::Guid::fromBase64(parsedJson.at(SINGUITY_ID_LABEL).get<std::string>()),
        xg::Guid::fromBase64(parsedJson.at(PLAYER_ID_LABEL).get<std::string>()),
        CommunicatedVector2D::fromJson(parsedJson.at(POSITION_LABEL)),
        CommunicatedVector2D::fromJson(parsedJson.at(SPEED_LABEL)),
        destinationJson.is_string() && destinationJson.get<std::string>() == NO_DESTINATION_VALUE
            ? Options::None<CommunicatedSinguityDestination>()
            : Options::Some(CommunicatedSinguityDestination::fromJson(destinationJson)),
        parsedJson.at(HEALTH_POINTS_LABEL).get<double>(),
        parsedJson.at(LAST_SHOOT_FRAME_COUNT_LABEL).get<long long>()
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

long long CommunicatedSinguity::lastShootFrameCount() const
{
    return _lastShootFameCount;
}

const std::string CommunicatedSinguity::NO_DESTINATION_VALUE = "n";
const std::string CommunicatedSinguity::SINGUITY_ID_LABEL = "i";
const std::string CommunicatedSinguity::PLAYER_ID_LABEL = "p";
const std::string CommunicatedSinguity::POSITION_LABEL = "o";
const std::string CommunicatedSinguity::SPEED_LABEL = "s";
const std::string CommunicatedSinguity::DESTINATION_LABEL = "d";
const std::string CommunicatedSinguity::HEALTH_POINTS_LABEL = "h";
const std::string CommunicatedSinguity::LAST_SHOOT_FRAME_COUNT_LABEL = "l";