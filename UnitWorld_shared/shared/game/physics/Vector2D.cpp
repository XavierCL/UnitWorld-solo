#include "Vector2D.h"

#include <cmath>

using namespace uw;

Vector2D::Vector2D(const double& x, const double& y) :
    _x(x),
    _y(y)
{}

double& Vector2D::x()
{
    return _x;
}

const double uw::Vector2D::x() const
{
    return _x;
}

void Vector2D::x(const double& newX)
{
    _x = newX;
}

double& Vector2D::y()
{
    return _y;
}

const double uw::Vector2D::y() const
{
    return _y;
}

void Vector2D::y(const double& newY)
{
    _y = newY;
}

const double Vector2D::distance(const Vector2D& other) const
{
    return sqrt(distanceSq(other));
}

const double Vector2D::distanceSq(const Vector2D& other) const
{
    return (_x - other._x) * (_x - other._x) + (_y - other._y) * (_y - other._y);
}

void Vector2D::operator+=(const Vector2D& other)
{
    _x += other._x;
    _y += other._y;
}

Vector2D Vector2D::operator+(const Vector2D& other)
{
    return Vector2D(_x + other._x, _y + other._y);
}

const double uw::Vector2D::module() const
{
    return sqrt(moduleSq());
}

const double uw::Vector2D::moduleSq() const
{
    return _x * _x + _y * _y;
}

Vector2D& uw::Vector2D::maxAt(const double & maxModule)
{
    if (_x * _x + _y * _y > maxModule * maxModule)
    {
        const auto absX = abs(_x);
        const auto absY = abs(_y);
        const auto xSign = absX / _x;
        const auto ySign = absY / _y;
        const auto xySum = absX + absY;
        const auto relativeX = absX / xySum;
        const auto relativeY = absY / xySum;
        _x = xSign * sqrt(relativeX * maxModule * maxModule);
        _y = ySign * sqrt((1 - relativeX) * maxModule * maxModule);
    }
    return *this;
}
