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
            _lastShootTimestamp(0)
        {}

        Shooter(const Vector2D& position, unsigned long long& lastShootTimestamp) :
            Unit(position),
            _lastShootTimestamp(lastShootTimestamp)
        {}

        Shooter(const xg::Guid& id, const Vector2D& position, const unsigned long long& lastShootTimestamp) :
            Unit(id, position),
            _lastShootTimestamp(lastShootTimestamp)
        {}

        void shootIfCan(std::shared_ptr<UnitWithHealthPoint> unitWithHealthPoint, unsigned long long frameTimestamp)
        {
            // Enemies can be marked as dead, but will only be removed from their player on the actualize phase
            // When enemies are already dead, keep the shooting for the next frame
            if (_lastShootTimestamp + shootTimelag() <= frameTimestamp
                && !unitWithHealthPoint->isDead()
                && position().distanceSq(unitWithHealthPoint->position()) < maxShootingRangeSq())
            {
                _lastShootTimestamp = frameTimestamp;
                unitWithHealthPoint->loseHealthPoint(firePower());
            }
        }

        unsigned long long lastShootTimestamp() const
        {
            return _lastShootTimestamp;
        }


    private:
        virtual double maxShootingRangeSq() const = 0;
        virtual unsigned long long shootTimelag() const = 0;
        virtual double firePower() const = 0;

        unsigned long long _lastShootTimestamp;
    };
}