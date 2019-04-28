#pragma once

#include "Unit.h"

namespace uw
{
    class MobileUnit : public Unit
    {
    public:
        ~MobileUnit();

        virtual void actualize();
        void setDestination(const Vector2D& destination);
    protected:
        MobileUnit(const MobileUnit& copy);
        MobileUnit(const Vector2D& initialPosition);

    private:
        void setMaximalAcceleration(const Vector2D& destination);
        const double stopDistanceFromTarget() const;
        const double stopDistanceFromTargetSq() const;
        Vector2D getBreakingAcceleration() const;
        void deleteDestination();
        virtual const double maximumSpeed() const = 0;
        virtual const double maximumAcceleration() const = 0;

        Vector2D* _destination;
        Vector2D _speed;
        Vector2D _acceleration;
    };
}