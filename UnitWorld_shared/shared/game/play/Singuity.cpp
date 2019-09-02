#include "Singuity.h"

using namespace uw;

Singuity::Singuity(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination, const bool& isBreakingForDestination, const double& healthPoints, const unsigned long long& lastShootTimestamp) :
    MobileUnit(id, position, speed, destination, isBreakingForDestination, healthPoints, lastShootTimestamp)
{}

Singuity::Singuity(const Vector2D& position):
    MobileUnit(position, maximumHealthPoints())
{}

Singuity::Singuity(const Singuity& other) :
    MobileUnit(other)
{}

double Singuity::maximumSpeed() const
{
    return 1.5;
}

double Singuity::maximumAcceleration() const
{
    return 0.02;
}

double Singuity::maximumHealthPoints() const
{
    return 20.0;
}

unsigned long long Singuity::shootTimelag() const
{
    return 500000000;
}

double Singuity::firePower() const
{
    return 5.0;
}