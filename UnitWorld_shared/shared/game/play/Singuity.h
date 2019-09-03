#pragma once

#include "UnitWithHealthPoint.h"
#include "Shooter.h"
#include "MobileUnit.h"

namespace uw
{
    class Singuity : virtual public MobileUnit, virtual public Shooter, virtual public UnitWithHealthPoint
    {
    public:
        Singuity(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination, const bool& isBreakingForDestination, const double& healthPoints, const unsigned long long& lastShootTimestamp);
        Singuity(const Vector2D& position, const Vector2D& speed);
        Singuity(const Vector2D& position);
        Singuity(const Singuity& other);

        static Singuity spawn(const Vector2D& position, const Vector2D& speed);
        static double spawnSpeed();

        double maximumSpeed() const override;
        double maximumAcceleration() const override;
        double maximumHealthPoint() const override;

    private:
        unsigned long long shootTimelag() const override;
        double firePower() const override;
        double maxShootingRangeSq() const override;
    };
}