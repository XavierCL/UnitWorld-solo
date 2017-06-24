#include "Rectangle.h"

using namespace uw;

Rectangle::Rectangle(const Point& upperLeftCorner,
	const unsigned int& width,
	const unsigned int& height) :
	_upperLeftCorner(upperLeftCorner),
	_width(width),
	_height(height)
{}

const bool Rectangle::contains(const Point& point) const
{
	return _upperLeftCorner.x() <= point.x() &&
		_upperLeftCorner.y() <= point.y() &&
		_upperLeftCorner.x() + _width > point.x() &&
		_upperLeftCorner.y() + _height > point.y();
}