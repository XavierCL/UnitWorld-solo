#pragma once

#include "Unit.h"

#include "commons/Option.hpp"

namespace uw
{
    class MobileUnit : public Unit
    {
    public:

        virtual void actualize();

        void setDestination(const Vector2D& destination);
        Option<Vector2D> destination() const;

        Vector2D speed() const;

    protected:
        MobileUnit(const MobileUnit& copy);
        MobileUnit(const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination);

    private:
        void setMaximalAcceleration(const Vector2D& destination);
        const double stopDistanceFromTarget() const;
        const double stopDistanceFromTargetSq() const;
        Vector2D getBreakingAcceleration() const;
        virtual const double maximumSpeed() const = 0;
        virtual const double maximumAcceleration() const = 0;

        Option<Vector2D> _destination;
        Vector2D _speed;
        Vector2D _acceleration;
    };
}