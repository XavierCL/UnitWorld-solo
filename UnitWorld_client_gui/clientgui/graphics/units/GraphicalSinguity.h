#pragma once

#include "GraphicalUnit.h"

#include "shared/game/play/Singuity.h"

namespace uw
{
    class GraphicalSinguity : public GraphicalUnit
    {
    public:
        GraphicalSinguity(std::shared_ptr<Singuity> self, const sf::Color& color);
    };
}