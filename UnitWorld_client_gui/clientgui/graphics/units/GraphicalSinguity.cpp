#include "GraphicalSinguity.h"

#include <cmath>

using namespace uw;

GraphicalSinguity::GraphicalSinguity(const Singuity& self, const sf::Color& color) :
    Singuity(self)
{
    const auto circleRadius(4.0);
    auto circleShape = std::make_shared<sf::CircleShape>(circleRadius);
    circleShape->setPosition(round(position().x() - circleRadius), round(position().y() - circleRadius));
    circleShape->setFillColor(color);
    _drawable = circleShape;
}