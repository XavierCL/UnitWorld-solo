#include "Unit.h"

using namespace uw;

Vector2& Unit::position()
{
	return _position;
}

void Unit::position(const Vector2& newPosition)
{
	_position = newPosition;
}

Unit::Unit(const Vector2& initialPosition) :
	_position(initialPosition)
{}