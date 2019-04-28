#pragma once

#include "GraphicalUnit.h"

#include "game/play/Singuity.h"

#include <cmath>

namespace uw
{
    class GraphicalSinguity : public GraphicalUnit, public Singuity
    {
    public:
        GraphicalSinguity(Singuity& self) :
            Singuity(self)
        {
            auto circleShape = std::make_shared<sf::CircleShape>(4);
            circleShape->setPosition(round(position().x()), round(position().y()));
            circleShape->setFillColor(sf::Color(0, 0, 255));
            _drawable = circleShape;
        }
    };
}