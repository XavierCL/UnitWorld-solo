#include "GraphicalSinguity.h"

#include <cmath>

using namespace uw;

GraphicalSinguity::GraphicalSinguity(const Singuity& self, const sf::Color& color) :
    Singuity(self)
{
    auto lifeRatio = self.healthPoints() / self.maximumHealthPoints();
    sf::Color lifeColor(round(color.r * lifeRatio), round(color.g * lifeRatio), round(color.b * lifeRatio));

    const auto circleRadius(3.0);
    auto circleShape = std::make_shared<sf::CircleShape>(circleRadius);
    circleShape->setPosition(round(position().x() - circleRadius), round(position().y() - circleRadius));
    circleShape->setFillColor(lifeColor);
    circleShape->setOutlineThickness(1.0);
    circleShape->setOutlineColor(color);
    _drawable = circleShape;
}