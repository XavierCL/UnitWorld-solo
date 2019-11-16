#pragma once

#include "Vector2D.h"
#include "CollidablePoint.h"

#include "commons/Guid.hpp"
#include "commons/Option.hpp"

namespace uw
{
    class CollisionDetector
    {
    public:
        virtual void updateAllCollidablePoints(std::shared_ptr<std::vector<CollidablePoint>> collidablePoints) = 0;

        virtual Option<xg::Guid> getClosest(const CollidablePoint& target) = 0;
    };
}