#include "Rectangle.h"

using namespace uw;

const bool Rectangle::contains(const Point& point) const
{
	return _upperLeftCorner.x() <= point.x() &&
		_upperLeftCorner.y() <= point.y() &&
		_upperLeftCorner.x() + _width >= point.x() &&
		_upperLeftCorner.y() + _height >= point.y();
}