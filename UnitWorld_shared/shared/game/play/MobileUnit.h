#pragma once

#include "Unit.h"

#include "commons/Option.hpp"

namespace uw
{
    class MobileUnit : virtual public Unit
    {
    public:

        Vector2D speed() const;
        Option<Vector2D> destination() const;
        bool isBreakingForDestination() const;

        virtual double maximumSpeed() const = 0;
        virtual double maximumAcceleration() const = 0;

        double stopDistanceFromTargetSq() const;
        Vector2D getMaximalAcceleration(const Vector2D& destination);
        Vector2D getBreakingAcceleration() const;
        Vector2D getSlowBreakingAcceleration() const;

        void setDestination(const Vector2D& destination);
        void clearDestination();
        void actualizeAcceleration(const Vector2D& instantaneousAcceleration);

        void setIsBreakingForDestination(const bool& isBreakingForDestination);

    protected:
        MobileUnit(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination, const bool& isBreakingForDestination);
        MobileUnit(const Vector2D& position, const Vector2D& speed);
        MobileUnit(const Vector2D& position);
        MobileUnit(const MobileUnit& copy);

    private:

        double stopDistanceFromTarget() const;

        Option<Vector2D> _destination;
        Vector2D _speed;
        bool _isBreakingForDestination;
    };
}