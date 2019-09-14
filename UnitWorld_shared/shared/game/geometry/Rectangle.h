#pragma once

#include "shared/game/geometry/Vector2D.h"

namespace uw
{
    class Rectangle
    {
    public:
        Rectangle(const Vector2D& corner1, const Vector2D& corner2);

        bool contains(const Vector2D& point) const;

        Rectangle smallerBy(const double& amount) const;

        Vector2D center() const;

        Vector2D size() const;

        Vector2D upperLeftCorner() const;

    private:
        Vector2D _upperLeftCorner;
        Vector2D _lowerRightCorner;
    };
}