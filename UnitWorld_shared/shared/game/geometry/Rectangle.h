#pragma once

#include "shared/game/geometry/Vector2D.h"
#include "shared/game/geometry/Circle.h"

namespace uw
{
    class Rectangle
    {
    public:
        Rectangle(const Vector2D& corner1, const Vector2D& corner2);

        bool contains(const Vector2D& point) const;

        bool intersectsWith(const Circle& circle) const;

        Vector2D closestPointTo(const Vector2D& other) const;

        Rectangle smallerBy(const double& amount) const;

        Vector2D center() const;

        Vector2D size() const;

        double width() const;

        Vector2D upperLeftCorner() const;

        Vector2D lowerRightCorner() const;

    private:
        Vector2D _upperLeftCorner;
        Vector2D _lowerRightCorner;
    };
}