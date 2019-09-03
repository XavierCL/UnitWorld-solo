#pragma once

#include "Unit.h"

namespace uw
{
    class UnitWithHealthPoint: virtual public Unit
    {
    public:

        UnitWithHealthPoint(const xg::Guid& id, const Vector2D& position, const double& healthPoint) :
            Unit(id, position),
            _healthPoint(healthPoint)
        {}

        UnitWithHealthPoint(const Vector2D& position, const double& maximumHealthPoint):
            Unit(position),
            _healthPoint(maximumHealthPoint)
        {}

        UnitWithHealthPoint(const UnitWithHealthPoint& copy) :
            Unit(copy),
            _healthPoint(copy._healthPoint)
        {}

        bool isDead() const
        {
            return _healthPoint <= 0.0;
        }

        double healthPoint() const
        {
            return _healthPoint;
        }

        virtual void loseHealthPoint(const double& healthPoint)
        {
            _healthPoint -= healthPoint;
        }

        virtual double maximumHealthPoint() const = 0;

    private:

        double _healthPoint;
    };
}