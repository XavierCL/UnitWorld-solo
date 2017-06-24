#include "Vector2.h"

using namespace uw;

Vector2::Vector2(const int& x, const int& y) :
	_x(x),
	_y(y)
{}

const int Vector2::x() const
{
	return _x;
}

void Vector2::x(const int& newX)
{
	_x = newX;
}

const int Vector2::y() const
{
	return _y;
}

void Vector2::y(const int& newY)
{
	_y = newY;
}