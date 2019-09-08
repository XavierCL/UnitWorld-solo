#pragma once

#include "Unit.h"

namespace uw
{
    class UnitWithHealthPoint: virtual public Unit
    {
    public:

        UnitWithHealthPoint(const xg::Guid& id, const Vector2D& position, const double& healthPoint) :
            Unit(id, position),
            _healthPoint(healthPoint),
            _hasBeenDead(healthPoint <= 0)
        {}

        UnitWithHealthPoint(const Vector2D& position, const double& maximumHealthPoint):
            Unit(position),
            _healthPoint(maximumHealthPoint),
            _hasBeenDead(maximumHealthPoint <= 0)
        {}

        UnitWithHealthPoint(const UnitWithHealthPoint& copy) :
            Unit(copy),
            _healthPoint(copy._healthPoint),
            _hasBeenDead(copy._hasBeenDead)
        {}

        bool isDead() const
        {
            return _hasBeenDead;
        }

        bool isAtMaximumHealth() const
        {
            return _healthPoint >= maximumHealthPoint() && !_hasBeenDead;
        }

        double healthPoint() const
        {
            return _healthPoint;
        }

        virtual void gainHealthPoint(const double& healthPoint)
        {
            _healthPoint += healthPoint;
        }

        virtual void loseHealthPoint(const double& healthPoint)
        {
            _healthPoint -= healthPoint;
            _hasBeenDead = _healthPoint <= 0 || _hasBeenDead;
        }

        virtual void kill()
        {
            _hasBeenDead = true;
        }

        virtual double maximumHealthPoint() const = 0;

    private:

        double _healthPoint;
        bool _hasBeenDead;
    };
}