#pragma once

#include "MobileUnit.h"

namespace uw
{
    class Singuity : public MobileUnit
    {
    public:
        Singuity(const Vector2D& initialPosition);

    private:
        const double maximumSpeed() const;
        const double maximumAcceleration() const;
    };
}