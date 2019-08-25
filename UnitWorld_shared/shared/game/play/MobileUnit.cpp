#include "MobileUnit.h"

#include <cmath>
#include <chrono>

using namespace uw;

void MobileUnit::actualize()
{
    _destination = _destination.filter([this](const Vector2D& destination) {
        _isBreakingForDestination = position().distanceSq(destination) < stopDistanceFromTargetSq() | _isBreakingForDestination;
        bool isReleaseBreakSpeed = _speed.moduleSq() < 0.01 * maximumSpeed() * maximumSpeed();

        if (!_isBreakingForDestination)
        {
            setMaximalAcceleration(destination);
            return true;
        }
        else if (!isReleaseBreakSpeed)
        {
            _acceleration = getBreakingAcceleration();
            return true;
        }
        else
        {
            return false;
        }
    }).orExecute([this] {
        _acceleration = getSlowBreakingAcceleration();
        _isBreakingForDestination = false;
    });

    _externalForce.foreach([this](auto externalForce) {
        _acceleration += externalForce.maxAt(maximumAcceleration());
        _acceleration.maxAt(maximumAcceleration());
    });

    _externalForce = Options::None<Vector2D>();

    _speed += _acceleration;
    _speed.maxAt(maximumSpeed());
    position(position() + _speed);
}

Vector2D MobileUnit::speed() const
{
    return _speed;
}

Option<Vector2D> MobileUnit::destination() const
{
    return _destination;
}

bool MobileUnit::isBreakingForDestination() const
{
    return _isBreakingForDestination;
}

unsigned long long MobileUnit::lastShootTimestamp() const
{
    return _lastShootTimestamp;
}

double MobileUnit::healthPoints() const
{
    return _healthPoints;
}

void MobileUnit::setDestination(const Vector2D& destination)
{
    _destination = Option<Vector2D>(destination);
}

void MobileUnit::setExternalForce(const Vector2D& outwardForcePosition)
{
    _externalForce = outwardForcePosition;
}

void MobileUnit::shoot(std::shared_ptr<MobileUnit> unitWithHealthPoint, unsigned long long frameTimestamp)
{
    _lastShootTimestamp = frameTimestamp;
    unitWithHealthPoint->loseHealthPoint(firePower());
}

bool MobileUnit::isDead() const
{
    return _healthPoints <= 0;
}

bool MobileUnit::canShoot() const
{
    return _lastShootTimestamp + shootTimelag() <= std::chrono::steady_clock::now().time_since_epoch().count();
}

MobileUnit::MobileUnit(const MobileUnit & copy) :
    Unit(copy),
    _speed(copy._speed),
    _acceleration(copy._acceleration),
    _isBreakingForDestination(copy._isBreakingForDestination),
    _destination(copy._destination),
    _healthPoints(copy._healthPoints),
    _lastShootTimestamp(copy._lastShootTimestamp)
{}

MobileUnit::MobileUnit(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination, const bool& isBreakingForDestination, const double& healthPoint, const unsigned long long& lastShootTimestamp) :
    Unit(id, position),
    _speed(speed),
    _destination(destination),
    _isBreakingForDestination(isBreakingForDestination),
    _healthPoints(healthPoint),
    _lastShootTimestamp(lastShootTimestamp)
{}

MobileUnit::MobileUnit(const Vector2D& position, const double& healthPoint):
    Unit(position),
    _speed(Vector2D()),
    _destination(),
    _isBreakingForDestination(false),
    _healthPoints(healthPoint),
    _lastShootTimestamp(0)
{}

void MobileUnit::setMaximalAcceleration(const Vector2D & destination)
{
    Vector2D expectedPosition(position().x() + _speed.x() * _speed.x() / (maximumAcceleration() * 1.2), position().y() + _speed.y() * _speed.y() / (maximumAcceleration() * 1.2));
    _acceleration = Vector2D(destination.x() - expectedPosition.x(), destination.y() - expectedPosition.y()).maxAt(maximumAcceleration());
}

const double MobileUnit::stopDistanceFromTarget() const
{
    return sqrt(stopDistanceFromTargetSq());
}

const double MobileUnit::stopDistanceFromTargetSq() const
{
    const auto speedModule = _speed.module();
    if (speedModule == 0)
    {
        return 0;
    }
    const auto relativeX = std::abs(_speed.x()) / speedModule;
    const auto relativeY = std::abs(_speed.y()) / speedModule;
    const auto accX = maximumAcceleration() * relativeX;
    const auto accY = maximumAcceleration() * relativeY;

    const auto numberOfMeterX = (_speed.x() / accX) * (_speed.x() / 2);
    const auto numberOfMeterY = (_speed.y() / accY) * (_speed.y() / 2);

    return Vector2D(numberOfMeterX, numberOfMeterY).moduleSq() + 1;
}

Vector2D MobileUnit::getBreakingAcceleration() const
{
    return Vector2D(-_speed.x(), -_speed.y()).maxAt(maximumAcceleration());
}

Vector2D MobileUnit::getSlowBreakingAcceleration() const
{
    return Vector2D(-_speed.x(), -_speed.y()).maxAt(maximumAcceleration() * 0.05);
}

void MobileUnit::loseHealthPoint(const double& healthPoint)
{
    _healthPoints -= healthPoint;
}