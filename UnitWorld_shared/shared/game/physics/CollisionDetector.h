#pragma once

#include "Vector2D.h"
#include "CollidablePoint.h"

#include "commons/Guid.hpp"

namespace uw
{
    class CollisionDetector
    {
    public:
        virtual void updateAllCollidablePoints(std::shared_ptr<std::vector<CollidablePoint>> collidablePoints) = 0;

        virtual xg::Guid getClosest(const Vector2D&) = 0;
    };
}