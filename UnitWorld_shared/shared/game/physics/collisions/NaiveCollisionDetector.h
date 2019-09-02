#pragma once

#include "CollisionDetector.h"

namespace uw
{
    class NaiveCollisionDetector: public CollisionDetector
    {
    public:
        virtual void updateAllCollidablePoints(std::vector<CollidablePoint> collidablePoints) override
        {
            _points = collidablePoints;
        }

        virtual Option<xg::Guid> getClosest(const CollidablePoint& target) override
        {
            Option<CollidablePoint> closestPoint;
            for (const auto& point : _points)
            {
                if (point.id() == target.id()) continue;
                closestPoint = closestPoint.map<CollidablePoint>([&target, &point](const auto& currentClosestPoint) {
                    return target.position().distanceSq(currentClosestPoint.position()) <= target.position().distanceSq(point.position())
                        ? currentClosestPoint
                        : point;
                }).orElse(Options::Some(point));
            }
            return closestPoint.map<xg::Guid>([](const auto& point) { return point.id(); });
        }

    private:
        std::vector<CollidablePoint> _points;
    };
}