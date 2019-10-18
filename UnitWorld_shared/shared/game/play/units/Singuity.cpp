#include "Singuity.h"

using namespace uw;

Singuity::Singuity(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination, const double& healthPoints, const unsigned long long& lastShootFrameCount) :
    Unit(id, position),
    MobileUnit(id, position, speed, destination),
    Shooter(id, position, lastShootFrameCount),
    UnitWithHealthPoint(id, position, healthPoints)
{}

Singuity::Singuity(const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination) :
    Unit(position),
    MobileUnit(position, speed, destination),
    Shooter(position),
    UnitWithHealthPoint(position, maximumHealthPoint())
{}

Singuity::Singuity(const Vector2D& position) :
    Unit(position),
    MobileUnit(position),
    Shooter(position),
    UnitWithHealthPoint(position, maximumHealthPoint())
{}

Singuity::Singuity(const Singuity& other) :
    Unit(other),
    MobileUnit(other),
    Shooter(other),
    UnitWithHealthPoint(other)
{}

Singuity Singuity::spawn(const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination)
{
    return Singuity(position, speed.maxAt(spawnSpeed()), destination);
}

double Singuity::spawnSpeed()
{
    return 0.05;
}

double Singuity::maximumSpeed() const
{
    return 4;
}

double Singuity::maximumAcceleration() const
{
    return 0.08;
}

double Singuity::maximumHealthPoint() const
{
    return 20.0;
}

unsigned long long Singuity::shootFramelag() const
{
    return 10;
}

double Singuity::firePower() const
{
    return 5.0;
}

double uw::Singuity::maxShootingRangeSq() const
{
    return 700.0;
}