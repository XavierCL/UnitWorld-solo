#pragma once

#include "MobileUnit.h"

namespace uw
{
    class Singuity : public MobileUnit
    {
    public:
        Singuity(const Vector2D& position, const Vector2D& speed, const Option<Vector2D>& destination);

    private:
        const double maximumSpeed() const;
        const double maximumAcceleration() const;
    };
}