#include "Rectangle.h"

#include <algorithm>

using namespace uw;
using namespace std;

Rectangle::Rectangle(const Vector2D& corner1,
    const Vector2D& corner2) :
    _upperLeftCorner(min(corner1.x(), corner2.x()), min(corner1.y(), corner2.y())),
    _lowerRightCorner(max(corner1.x(), corner2.x()), max(corner1.y(), corner2.y()))
{}

bool Rectangle::contains(const Vector2D& point) const
{
    return _upperLeftCorner.x() <= point.x() &&
        _upperLeftCorner.y() <= point.y() &&
        _lowerRightCorner.x() >= point.x() &&
        _lowerRightCorner.y() >= point.y();
}

Vector2D Rectangle::size() const
{
    return Vector2D(_lowerRightCorner.x() - _upperLeftCorner.x(), _lowerRightCorner.y() - _upperLeftCorner.y());
}

Vector2D Rectangle::center() const
{
    return Vector2D((_upperLeftCorner.x() + _lowerRightCorner.x()) / 2.0, (_upperLeftCorner.y() + _lowerRightCorner.y()) / 2.0);
}