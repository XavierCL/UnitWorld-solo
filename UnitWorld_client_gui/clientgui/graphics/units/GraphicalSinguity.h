#pragma once

#include "GraphicalUnit.h"

#include "shared/game/play/units/Singuity.h"

#include "shared/game/geometry/Circle.h"

namespace uw
{
    class GraphicalSinguity : public GraphicalUnit
    {
    public:
        GraphicalSinguity(std::shared_ptr<Singuity> self, const Circle& selfShape, const sf::Color& color);
    };
}