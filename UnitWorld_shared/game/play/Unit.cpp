#include "Unit.h"

using namespace uw;

Unit::Unit(const Point& position):
	_position(position)
{}

const Point Unit::position() const
{
	return _position;
}

void Unit::position(const Point& newPosition)
{
	_position = newPosition;
}