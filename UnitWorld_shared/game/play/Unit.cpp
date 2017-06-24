#include "Unit.h"

using namespace uw;

Unit::Unit(const Vector2& initialPosition):
	_position(initialPosition)
{}

const Vector2 Unit::position() const
{
	return _position;
}

void Unit::position(const Vector2& newPosition)
{
	_position = newPosition;
}