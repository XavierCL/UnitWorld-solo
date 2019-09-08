#include "MobileUnit.h"

#include <cmath>

using namespace uw;

void MobileUnit::actualize()
{
    _destination.filter([this](const Vector2D& destination) {
        return position().distanceSq(destination) > stopDistanceFromTargetSq();
    }).foreach([this](const Vector2D& destination) {
        setMaximalAcceleration(destination);
    }).orElse([this]() {
        _destination = Option<Vector2D>();
        _acceleration = getBreakingAcceleration();
    });

    _speed += _acceleration;
    _speed.maxAt(maximumSpeed());
    position(position() + _speed);
}

void MobileUnit::setDestination(const Vector2D& destination)
{
    _destination = Option<Vector2D>(destination);
}

Option<Vector2D> MobileUnit::destination() const
{
    return _destination;
}

Vector2D MobileUnit::speed() const
{
    return _speed;
}

MobileUnit::MobileUnit(const MobileUnit & copy) :
    Unit(copy)
{
    _destination = copy._destination;
    _speed = copy._speed;
}

MobileUnit::MobileUnit(const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination):
    Unit(position),
    _speed(speed),
    _destination(destination)
{}

void MobileUnit::setMaximalAcceleration(const Vector2D & destination)
{
    _acceleration = Vector2D(destination.x() - position().x(), destination.y() - position().y()).maxAt(maximumAcceleration());
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