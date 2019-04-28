#include "MobileUnit.h"

#include <cmath>

using namespace uw;

MobileUnit::~MobileUnit()
{
    delete _destination;
}

void MobileUnit::actualize()
{
    if (_destination && position().distanceSq(*_destination) > stopDistanceFromTargetSq())
    {
        setMaximalAcceleration(*_destination);
    }
    else
    {
        if (_destination)
        {
            deleteDestination();
        }
        _acceleration = getBreakingAcceleration();
    }
    _speed += _acceleration;
    _speed.maxAt(maximumSpeed());
    position() += _speed;
}

void MobileUnit::setDestination(const Vector2D& destination)
{
    delete _destination;
    _destination = new Vector2D(destination);
}

uw::MobileUnit::MobileUnit(const MobileUnit & copy) :
    Unit(copy)
{
    if (copy._destination)
    {
        _destination = new Vector2D(*copy._destination);
    }
    else
    {
        _destination = nullptr;
    }
    _speed = copy._speed;
}

MobileUnit::MobileUnit(const Vector2D& initialPosition) :
    Unit(initialPosition),
    _destination(nullptr)
{}

void uw::MobileUnit::setMaximalAcceleration(const Vector2D & destination)
{
    _acceleration = Vector2D(destination.x() - position().x(), destination.y() - position().y())
        .maxAt(maximumAcceleration());
}

Vector2D uw::MobileUnit::getBreakingAcceleration() const
{
    return Vector2D(-_speed.x(), -_speed.y()).maxAt(maximumAcceleration());
}

void MobileUnit::deleteDestination()
{
    delete _destination;
    _destination = nullptr;
}