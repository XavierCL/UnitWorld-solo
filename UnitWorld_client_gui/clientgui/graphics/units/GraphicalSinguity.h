#pragma once

#include "GraphicalUnit.h"

#include "shared/game/play/Singuity.h"

namespace uw
{
    class GraphicalSinguity : public GraphicalUnit, public Singuity
    {
    public:
        GraphicalSinguity(const Singuity& self, const sf::Color& color);
    };
}