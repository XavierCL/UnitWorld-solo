#include "Unit.h"

using namespace uw;

const Point Unit::position() const
{
	return _position;
}

void Unit::position(const Point& newPosition)
{
	_position = newPosition;
}