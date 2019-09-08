#include "Unit.h"

using namespace uw;

xg::Guid Unit::id() const
{
    return _id;
}

Vector2D Unit::position() const
{
    return _position;
}

void Unit::position(const Vector2D& newPosition)
{
    _position = newPosition;
}

Unit::Unit(const Vector2D& initialPosition) :
    _id(xg::newGuid()),
    _position(initialPosition)
{}