#include "MobileUnit.h"

#include <cmath>
#include <chrono>

using namespace uw;

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

void MobileUnit::setDestination(const Vector2D& destination)
{
    _destination = Option<Vector2D>(destination);
    _isBreakingForDestination = false;
}

void MobileUnit::clearDestination()
{
    _destination = Option<Vector2D>();
    _isBreakingForDestination = false;
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
    Vector2D stopDelta = _speed.atModule(stopDistanceFromTarget() * 0.98);
    Vector2D expectedPosition(position() + stopDelta);
    _acceleration = (destination - expectedPosition).maxAt(maximumAcceleration());
}

double MobileUnit::stopDistanceFromTargetSq() const
{
    const auto stopDistance = stopDistanceFromTarget();
    return stopDistance * stopDistance;
}

double MobileUnit::stopDistanceFromTarget() const
{
    return _speed.moduleSq() / (2.0 * maximumAcceleration());
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