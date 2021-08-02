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

bool Rectangle::intersectsWith(const Circle& circle) const
{
    const Vector2D rectanglePointClosestToCircle(closestPointTo(circle.center()));
    return circle.contains(rectanglePointClosestToCircle);
}

Vector2D Rectangle::closestPointTo(const Vector2D& other) const
{
    return Vector2D(
        std::max(std::min(_lowerRightCorner.x(), other.x()), _upperLeftCorner.x()),
        std::max(std::min(_lowerRightCorner.y(), other.y()), _upperLeftCorner.y())
    );
}

Rectangle Rectangle::smallerBy(const double& amount) const
{
    Vector2D newUpperLeftCorner = _upperLeftCorner + Vector2D(amount, amount);
    Vector2D newLowerRightCorner = _lowerRightCorner - Vector2D(amount, amount);

    if(newUpperLeftCorner.x() > newLowerRightCorner.x())
    {
        double cornerPositionX = (_upperLeftCorner.x() + _lowerRightCorner.x()) / 2.0;
        newUpperLeftCorner = Vector2D(cornerPositionX, _upperLeftCorner.y());
        newLowerRightCorner = Vector2D(cornerPositionX, _lowerRightCorner.y());
    }
    if (newUpperLeftCorner.y() > newLowerRightCorner.y())
    {
        double cornerPositionY = (_upperLeftCorner.y() + _lowerRightCorner.y()) / 2.0;
        newUpperLeftCorner = Vector2D(newUpperLeftCorner.x(), cornerPositionY);
        newLowerRightCorner = Vector2D(newLowerRightCorner.x(), cornerPositionY);
    }

    return Rectangle(newUpperLeftCorner, newLowerRightCorner);
}

Vector2D Rectangle::center() const
{
    return Vector2D((_upperLeftCorner.x() + _lowerRightCorner.x()) / 2.0, (_upperLeftCorner.y() + _lowerRightCorner.y()) / 2.0);
}

Vector2D Rectangle::size() const
{
    return Vector2D(_lowerRightCorner.x() - _upperLeftCorner.x(), _lowerRightCorner.y() - _upperLeftCorner.y());
}

double Rectangle::width() const
{
    return size().x();
}

Vector2D Rectangle::upperLeftCorner() const
{
    return _upperLeftCorner;
}

Vector2D Rectangle::lowerRightCorner() const
{
    return _lowerRightCorner;
}