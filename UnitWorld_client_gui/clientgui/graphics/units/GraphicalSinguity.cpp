#include "GraphicalSinguity.h"

#include <cmath>

using namespace uw;

GraphicalSinguity::GraphicalSinguity(const Singuity& self) :
    Singuity(self)
{
    const auto circleRadius(4.0);
    auto circleShape = std::make_shared<sf::CircleShape>(circleRadius);
    circleShape->setPosition(round(position().x() - circleRadius), round(position().y() - circleRadius));
    circleShape->setFillColor(sf::Color(0, 0, 255));
    _drawable = circleShape;
}