#include "Point.h"

using namespace uw;

const int Point::x() const
{
	return _x;
}

void Point::x(const int& newX)
{
	_x = newX;
}

const int Point::y() const
{
	return _y;
}

void Point::y(const int& newY)
{
	_y = newY;
}