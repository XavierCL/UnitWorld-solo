#include "GraphicalSinguity.h"

#include <cmath>

using namespace uw;

GraphicalSinguity::GraphicalSinguity(const Singuity& self) :
    Singuity(self)
{
    auto circleShape = std::make_shared<sf::CircleShape>(4);
    circleShape->setPosition(round(position().x()), round(position().y()));
    circleShape->setFillColor(sf::Color(0, 0, 255));
    _drawable = circleShape;
}