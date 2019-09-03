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

double MobileUnit::stopDistanceFromTargetSq() const
{
    const auto stopDistance = stopDistanceFromTarget();
    return stopDistance * stopDistance;
}

Vector2D MobileUnit::getMaximalAcceleration(const Vector2D & destination)
{
    Vector2D stopDelta = _speed.atModule(stopDistanceFromTarget() * 0.98);
    Vector2D expectedPosition(position() + stopDelta);
    return (destination - expectedPosition).maxAt(maximumAcceleration());
}

Vector2D MobileUnit::getBreakingAcceleration() const
{
    return Vector2D(-_speed.x(), -_speed.y()).maxAt(maximumAcceleration());
}

Vector2D MobileUnit::getSlowBreakingAcceleration() const
{
    return Vector2D(-_speed.x(), -_speed.y()).maxAt(maximumAcceleration() * 0.05);
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

void MobileUnit::actualizeAcceleration(const Vector2D& instantaneousAcceleration)
{
    _speed += instantaneousAcceleration;
    position(position() + _speed);
}

void MobileUnit::setIsBreakingForDestination(const bool& isBreakingForDestination)
{
    _isBreakingForDestination = isBreakingForDestination;
}

MobileUnit::MobileUnit(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination, const bool& isBreakingForDestination):
    Unit(id, position),
    _speed(speed),
    _destination(destination),
    _isBreakingForDestination(isBreakingForDestination)
{}

MobileUnit::MobileUnit(const Vector2D& position, const Vector2D& speed):
    Unit(position),
    _speed(speed),
    _destination(Options::None<Vector2D>()),
    _isBreakingForDestination(false)
{}

MobileUnit::MobileUnit(const Vector2D& position):
    Unit(position),
    _speed(0.0),
    _destination(Options::None<Vector2D>()),
    _isBreakingForDestination(false)
{}

MobileUnit::MobileUnit(const MobileUnit& copy) :
    Unit(copy),
    _speed(copy._speed),
    _destination(copy._destination),
    _isBreakingForDestination(copy._isBreakingForDestination)
{}

double MobileUnit::stopDistanceFromTarget() const
{
    return _speed.moduleSq() / (2.0 * maximumAcceleration());
}