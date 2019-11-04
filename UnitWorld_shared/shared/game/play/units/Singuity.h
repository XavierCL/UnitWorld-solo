#pragma once

#include "UnitWithHealthPoint.h"
#include "Shooter.h"
#include "MobileUnit.h"

namespace uw
{
    class Singuity : virtual public MobileUnit, virtual public Shooter, virtual public UnitWithHealthPoint
    {
    public:
        Singuity(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination, const double& healthPoints, const unsigned long long& lastShootFrameCount);
        Singuity(const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination);
        Singuity(const Vector2D& position);
        Singuity(const Singuity& other);

        static Singuity spawn(const Vector2D& position, const Vector2D& speed, const Option<MobileUnitDestination>& destination);
        static double spawnSpeed();

        double maximumSpeed() const override;
        double maximumAcceleration() const override;
        double maximumHealthPoint() const override;

        double reguvenatingHealth() const
        {
            return 50.0;
        }

    private:
        unsigned long long shootFramelag() const override;
        double firePower() const override;
        double maxShootingRangeSq() const override;

        static double singuityRadius()
        {
            return 4.0;
        }
    };
}