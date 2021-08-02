#pragma once

#include "UnitWithHealthPoint.h"

#include "shared/game/geometry/Circle.h"

#include <memory>

namespace uw
{
    class Shooter: virtual public Unit
    {
    public:
        Shooter(const Vector2D& position, const double& radius) :
            Unit(position, radius),
            _lastShootFameCount(0)
        {}

        Shooter(const Vector2D& position, const double& radius, long long& lastShootFrameCount) :
            Unit(position, radius),
            _lastShootFameCount(lastShootFrameCount)
        {}

        Shooter(const xg::Guid& id, const Vector2D& position, const double& radius, const long long& lastShootFrameCount) :
            Unit(id, position, radius),
            _lastShootFameCount(lastShootFrameCount)
        {}

        void shootIfCan(std::shared_ptr<UnitWithHealthPoint> unitWithHealthPoint, long long frameCount)
        {
            if (canShoot(unitWithHealthPoint, frameCount))
            {
                _lastShootFameCount = frameCount;
                unitWithHealthPoint->loseHealthPoint(firePower());
            }
        }

        bool canShoot(std::shared_ptr<UnitWithHealthPoint> unitWithHealthPoint, long long frameCount)
        {
            // Enemies can be marked as dead, but will only be removed from their player on the actualize phase
            // When enemies are already dead, keep the shooting for the next frame
            return _lastShootFameCount + shootFramelag() <= frameCount
                && !unitWithHealthPoint->isDead()
                && Circle(position(), maxShootingRange() + radius() + unitWithHealthPoint->radius()).contains(unitWithHealthPoint->position());
        }

        long long lastShootFrameCount() const
        {
            return _lastShootFameCount;
        }


    private:
        virtual double maxShootingRange() const = 0;
        virtual long long shootFramelag() const = 0;
        virtual double firePower() const = 0;

        long long _lastShootFameCount;
    };
}