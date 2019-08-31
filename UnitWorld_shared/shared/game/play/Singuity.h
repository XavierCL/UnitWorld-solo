#pragma once

#include "MobileUnit.h"

namespace uw
{
    class Singuity : public MobileUnit
    {
    public:
        Singuity(const xg::Guid& id, const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination, const bool& isBreakingForDestination, const double& healthPoints, const unsigned long long& lastShootTimestamp);
        Singuity(const Vector2D& position);
        Singuity(const Singuity& other);

        double maximumHealthPoints() const;

    private:
        double maximumSpeed() const override;
        double maximumAcceleration() const override;
        unsigned long long shootTimelag() const override;
        double firePower() const override;

    };
}