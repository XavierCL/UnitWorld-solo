#pragma once

#include "UnitWithHealthPoint.h"

#include <memory>

namespace uw
{
    class Shooter: virtual public Unit
    {
    public:
        Shooter(const Vector2D& position) :
            Unit(position),
            _lastShootFameCount(0)
        {}

        Shooter(const Vector2D& position, unsigned long long& lastShootFrameCount) :
            Unit(position),
            _lastShootFameCount(lastShootFrameCount)
        {}

        Shooter(const xg::Guid& id, const Vector2D& position, const unsigned long long& lastShootFrameCount) :
            Unit(id, position),
            _lastShootFameCount(lastShootFrameCount)
        {}

        void shootIfCan(std::shared_ptr<UnitWithHealthPoint> unitWithHealthPoint, unsigned long long frameCount)
        {
            // Enemies can be marked as dead, but will only be removed from their player on the actualize phase
            // When enemies are already dead, keep the shooting for the next frame
            if (_lastShootFameCount + shootFramelag() <= frameCount
                && !unitWithHealthPoint->isDead()
                && position().distanceSq(unitWithHealthPoint->position()) < maxShootingRangeSq())
            {
                _lastShootFameCount = frameCount;
                unitWithHealthPoint->loseHealthPoint(firePower());
            }
        }

        unsigned long long lastShootFrameCount() const
        {
            return _lastShootFameCount;
        }


    private:
        virtual double maxShootingRangeSq() const = 0;
        virtual unsigned long long shootFramelag() const = 0;
        virtual double firePower() const = 0;

        unsigned long long _lastShootFameCount;
    };
}