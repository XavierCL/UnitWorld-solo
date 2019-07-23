#include "Singuity.h"

using namespace uw;

Singuity::Singuity(const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination):
    MobileUnit(position, speed, destination)
{}

const double Singuity::maximumSpeed() const
{
    return 1.5;
}

const double Singuity::maximumAcceleration() const
{
    return 0.02;
}