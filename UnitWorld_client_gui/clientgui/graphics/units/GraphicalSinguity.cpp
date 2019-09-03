#include "GraphicalSinguity.h"

#include <cmath>

using namespace uw;

GraphicalSinguity::GraphicalSinguity(std::shared_ptr<Singuity> self, const sf::Color& color)
{
    auto lifeRatio = self->healthPoint() / self->maximumHealthPoint();
    sf::Color lifeColor(round(color.r * lifeRatio), round(color.g * lifeRatio), round(color.b * lifeRatio));

    const auto circleRadius(3.0);
    auto circleShape = std::make_shared<sf::CircleShape>(circleRadius);
    circleShape->setPosition(round(self->position().x() - circleRadius), round(self->position().y() - circleRadius));
    circleShape->setFillColor(lifeColor);
    circleShape->setOutlineThickness(1.0);
    circleShape->setOutlineColor(color);
    _drawable = circleShape;
}