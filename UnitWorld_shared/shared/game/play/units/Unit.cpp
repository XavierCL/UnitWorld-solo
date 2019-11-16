#include "Unit.h"

using namespace uw;

Unit::Unit(const xg::Guid& id, const Vector2D& initialPosition) :
    _id(id),
    _position(initialPosition)
{}

Unit::Unit(const Vector2D& initialPosition) :
    _id(xg::newGuid()),
    _position(initialPosition)
{}

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