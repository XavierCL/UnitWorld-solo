#include "Point.h"

using namespace uw;

Point::Point(const int& x, const int& y) :
	_x(x),
	_y(y)
{}

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