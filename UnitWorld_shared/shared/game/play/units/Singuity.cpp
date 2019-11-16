#include "Singuity.h"

using namespace uw;

Singuity::Singuity(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<std::variant<Vector2D, SpawnerDestination>>& destination, const double& healthPoints, const unsigned long long& lastShootTimestamp) :
    Unit(id, position),
    MobileUnit(id, position, speed, destination),
    Shooter(id, position, lastShootTimestamp),
    UnitWithHealthPoint(id, position, healthPoints)
{}

Singuity::Singuity(const Vector2D& position, const Vector2D& speed) :
    Unit(position),
    MobileUnit(position, speed),
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

Singuity Singuity::spawn(const Vector2D& position, const Vector2D& speed)
{
    return Singuity(position, speed.maxAt(spawnSpeed()));
}

double Singuity::spawnSpeed()
{
    return 0.05;
}

double Singuity::maximumSpeed() const
{
    return 0.5;
}

double Singuity::maximumAcceleration() const
{
    return 0.02;
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