#pragma once

#include "Unit.h"

#include "commons/Option.hpp"

namespace uw
{
    class MobileUnit : public Unit
    {
    public:

        virtual void actualize();

        Vector2D speed() const;
        Option<Vector2D> destination() const;
        bool isBreakingForDestination() const;
        unsigned long long lastShootTimestamp() const;
        double healthPoints() const;

        void setDestination(const Vector2D& destination);
        void setExternalForce(const Vector2D& outwardForcePosition);

        void shoot(std::shared_ptr<MobileUnit> unitWithHealthPoint, unsigned long long frameTimestamp);
        bool isDead() const;
        bool canShoot() const;

    protected:
        MobileUnit(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination, const bool& isBreakingForDestination, const double& healthPoint, const unsigned long long& lastShootTimestamp);
        MobileUnit(const Vector2D& position, const double& healthPoint);
        MobileUnit(const MobileUnit& copy);

    private:
        void setMaximalAcceleration(const Vector2D& destination);
        double stopDistanceFromTargetSq() const;
        double stopDistanceFromTarget() const;
        Vector2D getBreakingAcceleration() const;
        Vector2D getSlowBreakingAcceleration() const;
        void loseHealthPoint(const double& healthPoint);

        virtual double maximumSpeed() const = 0;
        virtual double maximumAcceleration() const = 0;
        virtual unsigned long long shootTimelag() const = 0;
        virtual double firePower() const = 0;

        Option<Vector2D> _destination;
        Vector2D _speed;
        Vector2D _acceleration;
        Option<Vector2D> _externalForce;
        bool _isBreakingForDestination;
        double _healthPoints;
        unsigned long long _lastShootTimestamp;
    };
}