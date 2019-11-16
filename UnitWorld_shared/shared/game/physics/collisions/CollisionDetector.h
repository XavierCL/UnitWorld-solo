#pragma once

#include "shared/game/geometry/Vector2D.h"
#include "CollidablePoint.h"

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

namespace uw
{
    class CollisionDetector
    {
    public:
        virtual void updateAllCollidablePoints(const std::vector<CollidablePoint>& collidablePoint) = 0;

        virtual Option<xg::Guid> getClosest(const CollidablePoint& target) = 0;
    };
}