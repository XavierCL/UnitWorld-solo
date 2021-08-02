#pragma once

#include "Unit.h"

namespace uw
{
    class UnitWithHealthPoint: virtual public Unit
    {
    public:

        UnitWithHealthPoint(const xg::Guid& id, const Vector2D& position, const double& radius, const double& healthPoint) :
            Unit(id, position, radius),
            _healthPoint(healthPoint)
        {}

        UnitWithHealthPoint(const Vector2D& position, const double& radius, const double& maximumHealthPoint):
            Unit(position, radius),
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

        bool isAtMaximumHealth() const
        {
            return _healthPoint >= maximumHealthPoint();
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
        }

        virtual void makeHealthPointNone()
        {
            _healthPoint = 0.0;
        }

        virtual double maximumHealthPoint() const = 0;

    private:

        double _healthPoint;
    };
}