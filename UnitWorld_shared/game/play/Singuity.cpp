#include "Singuity.h"

using namespace uw;

Singuity::Singuity(const Vector2D& initialPosition) :
    MobileUnit(initialPosition)
{}

const double Singuity::maximumSpeed() const
{
    return 1.5;
}

const double Singuity::maximumAcceleration() const
{
    return 0.02;
}