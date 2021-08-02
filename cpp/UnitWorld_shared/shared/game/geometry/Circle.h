#pragma once

#include "Vector2D.h"

namespace uw
{
    class Circle
    {
    public:
        Circle(const Vector2D& center, const double& radius) :
            _center(center),
            _radius(radius)
        {}

        bool contains(const Vector2D& point) const
        {
            return sq(_center.x() - point.x()) + sq(_center.y() - point.y()) <= sq(_radius);
        }

        Vector2D center() const
        {
            return _center;
        }

        double radius() const
        {
            return _radius;
        }

    private:

        static double sq(const double& value)
        {
            return value * value;
        }

        const Vector2D _center;
        const double _radius;
    };
}