#include "Unit.h"

using namespace uw;

Vector2D& Unit::position()
{
    return _position;
}

void Unit::position(const Vector2D& newPosition)
{
    _position = newPosition;
}

Unit::Unit(const Vector2D& initialPosition) :
    _position(initialPosition)
{}