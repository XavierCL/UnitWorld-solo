#include "Singuity.h"

using namespace uw;

Singuity::Singuity(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination, const double& healthPoints, const long long& lastShootFrameCount) :
    Unit(id, position, singuityRadius()),
    MobileUnit(id, position, singuityRadius(), speed, destination),
    Shooter(id, position, lastShootFrameCount, singuityRadius()),
    UnitWithHealthPoint(id, position, singuityRadius(), healthPoints)
{}

Singuity::Singuity(const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination) :
    Unit(position, singuityRadius()),
    MobileUnit(position, singuityRadius(), speed, destination),
    Shooter(position, singuityRadius()),
    UnitWithHealthPoint(position, singuityRadius(), maximumHealthPoint())
{}

Singuity::Singuity(const Vector2D& position) :
    Unit(position, singuityRadius()),
    MobileUnit(position, singuityRadius()),
    Shooter(position, singuityRadius()),
    UnitWithHealthPoint(position, singuityRadius(), maximumHealthPoint())
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

long long Singuity::shootFramelag() const
{
    return 6;
}

double Singuity::firePower() const
{
    return 5.0;
}

double uw::Singuity::maxShootingRange() const
{
    return 18.0;
}