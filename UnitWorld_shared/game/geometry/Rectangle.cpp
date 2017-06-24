#include "Rectangle.h"

#include <algorithm>

using namespace uw;
using namespace std;

Rectangle::Rectangle(const Vector2& corner1,
	const Vector2& corner2):
	_upperLeftCorner(min(corner1.x(), corner2.x()), min(corner1.y(), corner2.y())),
	_lowerRightCorner(max(corner1.x(), corner2.x()), max(corner1.y(), corner2.y()))
{}

const bool Rectangle::contains(const Vector2& point) const
{
	return _upperLeftCorner.x() <= point.x() &&
		_upperLeftCorner.y() <= point.y() &&
		_lowerRightCorner.x() >= point.x() &&
		_lowerRightCorner.y() >= point.y();
}